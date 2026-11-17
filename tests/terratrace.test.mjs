import test from 'node:test'
import assert from 'node:assert/strict'
import { DEMO, ipa, isHeavy, priority, statistics, validLot, exportRows, toCsv } from '../src/lib/terratrace.ts'

test('IPA reproduz os sete exemplos do Apêndice A', () => {
  assert.deepEqual(DEMO.map(ipa), [0, 50, 50, 100, 0, 20, 40])
  assert.ok(DEMO.every(validLot))
})
test('IPA cobre todas as combinações e os limites 30, 40 e 70', () => {
  for (const rastreado of [false, true]) for (const licenca_ok of [false, true]) for (const elemento of ['Nd', 'Tb']) for (const teor_treo of [29.9, 30]) {
    assert.equal(ipa({ rastreado, licenca_ok, elemento, teor_treo }), (rastreado ? 0 : 40) + (licenca_ok ? 0 : 30) + (elemento === 'Tb' ? 20 : 0) + (teor_treo < 30 ? 10 : 0))
  }
  assert.deepEqual([0, 39, 40, 69, 70, 100].map(priority), ['Baixa', 'Baixa', 'Média', 'Média', 'Alta', 'Alta'])
  assert.equal(isHeavy('Y'), true)
  assert.equal(isHeavy('Sc'), false)
})
test('estatísticas calculam massa e teor ponderado, inclusive sem lotes', () => {
  const stats = statistics(DEMO)
  assert.ok(Math.abs(stats.mass - 165.4) < 0.00001)
  assert.ok(Math.abs(stats.weighted - 7790.5 / 165.4) < 0.00001)
  assert.equal(stats.high, 1)
  assert.equal(stats.traced, 4 / 7 * 100)
  assert.equal(stats.licensed, 5 / 7 * 100)
  assert.deepEqual(statistics([]), { mass: 0, weighted: 0, traced: 0, licensed: 0, heavy: 0, high: 0 })
})
test('validação rejeita números fora dos limites e campos inválidos', () => {
  for (const changes of [{ massa_t: 0 }, { massa_t: NaN }, { massa_t: 1000001 }, { teor_treo: -1 }, { teor_treo: 101 }, { uf: 'XX' }, { elemento: 'Fe' }, { origem: '   ' }, { destino: '' }, { id: 1.5 }, { rastreado: 1 }]) assert.equal(validLot({ ...DEMO[0], ...changes }), false)
  assert.ok(validLot({ ...DEMO[0], massa_t: 0.001, teor_treo: 0 }))
})
test('exportação mantém IPA, grupo e caracteres; CSV escapa aspas e fórmulas', () => {
  const json = exportRows(DEMO)
  assert.equal(json[3].ipa, 100)
  assert.equal(json[3].classe, 'ALTA')
  assert.equal(json[1].grupo, 'pesado')
  assert.equal(json[0]._id, 1)
  const csv = toCsv([{ ...DEMO[0], origem: '=SOMA(1,2)', destino: 'Destino "Sul", BR' }])
  assert.ok(csv.includes('"\'=SOMA(1,2)"'))
  assert.ok(csv.includes('"Destino ""Sul"", BR"'))
  assert.equal(toCsv([]).split('\r\n').length, 2)
})
