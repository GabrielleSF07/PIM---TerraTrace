# Imagens do TerraTrace

Geradas com a ferramenta integrada Imagegen, sem uso de CLI ou API externa pelo projeto. São ilustrações, não registros de uma operação minerária real. O banner `Mídia.jpg`, fornecido pelo usuário, orientou a identidade visual. Os arquivos ficam no próprio projeto.

## Abertura

Arquivo: `public/images/mineracao-terratrace.png`

Prompt utilizado:

> Use case: photorealistic-natural. Create a website hero photograph for a Brazilian university project called TerraTrace about rare earth mineral traceability. Wide landscape 1536x1024 composition, realistic editorial photograph of a large open-pit mine with terraced ochre mineral rock benches, green forested Brazilian hills in the background, natural pale blue cloudy sky, one yellow mining haul truck in the lower right providing scale. Environment should resemble the mining landscape visible at top right of the supplied academic banner, without copying any of its lettering or graphics. Strong real geological detail, natural daylight, believable industrial scene, forest green and grey earthy tones, no fantasy futuristic visual effects. Right two thirds contain the mine, upper left relatively quiet for cropping; image used in website with dark green heading panel at left. No text, no typography, no logos, no infographic, no watermark. This is an illustrative generated scene, not documentation of any real mine.

## Introdução

Arquivo: `public/images/minerais-sustentabilidade.png`

Prompt utilizado:

> Use case: photorealistic-natural. Asset type: editorial website image for TerraTrace, a Brazilian academic research project on traceability of rare earth mineral concentrates. Landscape 1536x1024. Close-up photograph of a small young native plant with bright green healthy leaves growing in dark soil nestled between rough black and dark grey mineral ore rocks, flecks of natural metallic mineral grains, believable botanical and geological texture. Foreground in sharp focus, soft natural daylight, out-of-focus forest and muted open-pit mineral rock terraces in distance. Deep forest green, charcoal, pale neutral sky, realistic restrained photographic color. Inspired by the seedling-and-mineral visual in the supplied academic TerraTrace banner. No infographic, no lettering, no watermark, no logos, no 3D render aesthetic, no glittering fantasy crystals, no artificial lens flare, no arrows, no people. This symbolic illustration is not a claim that mining is automatically sustainable.

## Execução e verificação

- `npm.cmd run dev`: desenvolvimento local.
- `npm.cmd test`: testes da lógica do protótipo.
- `npm.cmd run lint`: análise estática.
- `npm.cmd run build`: TypeScript e build de produção.

O programa C foi extraído do Apêndice A do documento fornecido e está disponível para download em `public/terratrace.c`. O site reproduz a lógica em TypeScript; não compila nem executa C no navegador. PostgreSQL, MongoDB e MQTT/TLS são apresentados como componentes planejados no PIM.
