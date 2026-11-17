export const ELEMENTS = ['La', 'Ce', 'Pr', 'Nd', 'Pm', 'Sm', 'Eu', 'Gd', 'Tb', 'Dy', 'Ho', 'Er', 'Tm', 'Yb', 'Lu', 'Sc', 'Y']
export const UFS = ['AC', 'AL', 'AP', 'AM', 'BA', 'CE', 'DF', 'ES', 'GO', 'MA', 'MT', 'MS', 'MG', 'PA', 'PB', 'PR', 'PE', 'PI', 'RJ', 'RN', 'RS', 'RO', 'RR', 'SC', 'SP', 'SE', 'TO']
export type Lot = { id: number; origem: string; uf: string; elemento: string; massa_t: number; teor_treo: number; rastreado: boolean; licenca_ok: boolean; destino: string }
export const isHeavy = (element: string) => ['Gd', 'Tb', 'Dy', 'Ho', 'Er', 'Tm', 'Yb', 'Lu', 'Y'].includes(element)
export function ipa(lot: Pick<Lot, 'rastreado' | 'licenca_ok' | 'elemento' | 'teor_treo'>) {
  return (!lot.rastreado ? 40 : 0) + (!lot.licenca_ok ? 30 : 0) + (isHeavy(lot.elemento) ? 20 : 0) + (lot.teor_treo < 30 ? 10 : 0)
}
export const priority = (score: number) => score >= 70 ? 'Alta' : score >= 40 ? 'Média' : 'Baixa'
export const DEMO: Lot[] = [
  { id: 1, origem: 'Projeto Alfa (demo)', uf: 'GO', elemento: 'Nd', massa_t: 42, teor_treo: 48.5, rastreado: true, licenca_ok: true, destino: 'Refinaria Sul' },
  { id: 2, origem: 'Projeto Beta (demo)', uf: 'MG', elemento: 'Dy', massa_t: 3.2, teor_treo: 35, rastreado: true, licenca_ok: false, destino: 'Laboratório Norte' },
  { id: 3, origem: 'Projeto Gama (demo)', uf: 'BA', elemento: 'Pr', massa_t: 18.7, teor_treo: 27, rastreado: false, licenca_ok: true, destino: 'Fábrica de Ímãs' },
  { id: 4, origem: 'Projeto Delta (demo)', uf: 'GO', elemento: 'Tb', massa_t: 1.1, teor_treo: 22, rastreado: false, licenca_ok: false, destino: 'Exportação' },
  { id: 5, origem: 'Projeto Epsilon (demo)', uf: 'MG', elemento: 'Ce', massa_t: 75, teor_treo: 55, rastreado: true, licenca_ok: true, destino: 'Polimento Ind.' },
  { id: 6, origem: 'Projeto Zeta (demo)', uf: 'AM', elemento: 'Y', massa_t: 5.4, teor_treo: 31, rastreado: true, licenca_ok: true, destino: 'Fósforos Ltda' },
  { id: 7, origem: 'Projeto Eta (demo)', uf: 'GO', elemento: 'La', massa_t: 20, teor_treo: 41, rastreado: false, licenca_ok: true, destino: 'Baterias SA' },
]
export function statistics(lots: Lot[]) {
  const mass = lots.reduce((sum, lot) => sum + lot.massa_t, 0)
  return {
    mass,
    weighted: mass ? lots.reduce((sum, lot) => sum + lot.massa_t * lot.teor_treo, 0) / mass : 0,
    traced: lots.length ? lots.filter(lot => lot.rastreado).length / lots.length * 100 : 0,
    licensed: lots.length ? lots.filter(lot => lot.licenca_ok).length / lots.length * 100 : 0,
    heavy: mass ? lots.filter(lot => isHeavy(lot.elemento)).reduce((sum, lot) => sum + lot.massa_t, 0) / mass * 100 : 0,
    high: lots.filter(lot => ipa(lot) >= 70).length,
  }
}
export function validLot(value: unknown): value is Lot {
  if (!value || typeof value !== 'object') return false
  const lot = value as Lot
  return Number.isInteger(lot.id) && lot.id > 0 && typeof lot.origem === 'string' && lot.origem.trim().length > 0 && lot.origem.length <= 39 && typeof lot.destino === 'string' && lot.destino.trim().length > 0 && lot.destino.length <= 39 && UFS.includes(lot.uf) && ELEMENTS.includes(lot.elemento) && Number.isFinite(lot.massa_t) && lot.massa_t >= 0.001 && lot.massa_t <= 1000000 && Number.isFinite(lot.teor_treo) && lot.teor_treo >= 0 && lot.teor_treo <= 100 && typeof lot.rastreado === 'boolean' && typeof lot.licenca_ok === 'boolean'
}
export function exportRows(lots: Lot[]) {
  return lots.map(({ id, ...lot }) => ({ _id: id, ...lot, grupo: isHeavy(lot.elemento) ? 'pesado' : 'leve', ipa: ipa(lot), classe: priority(ipa(lot)).normalize('NFD').replace(/[\u0300-\u036f]/g, '').toUpperCase() }))
}
export function toCsv(lots: Lot[]) {
  const cell = (value: string | number) => typeof value === 'number' ? String(value) : `"${(/^[=+@\-\t\r]/.test(value) ? "'" : '') + value.replaceAll('"', '""')}"`
  return 'id,origem,uf,elemento,massa_t,teor_treo,rastreado,licenca_ok,destino,ipa,classe\r\n' + lots.map(l => [l.id, l.origem, l.uf, l.elemento, l.massa_t, l.teor_treo, Number(l.rastreado), Number(l.licenca_ok), l.destino, ipa(l), priority(ipa(l))].map(cell).join(',')).join('\r\n')
}
