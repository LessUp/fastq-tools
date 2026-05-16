import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const diagramCss = readFileSync(new URL('../.vitepress/theme/styles/diagrams.css', import.meta.url), 'utf8')
const diagramFrameSource = readFileSync(
  new URL('../.vitepress/theme/components/DiagramFrame.vue', import.meta.url),
  'utf8',
)

const adoptedPages = [
  '../en/architecture/index.md',
  '../zh/architecture/index.md',
  '../en/knowledge-map/index.md',
  '../zh/knowledge-map/index.md',
  '../en/performance/index.md',
  '../zh/performance/index.md',
].map((relativePath) => ({
  relativePath,
  source: readFileSync(new URL(relativePath, import.meta.url), 'utf8'),
}))

test('diagram css defines shared light and dark tokens', () => {
  for (const token of ['--fq-diagram-bg', '--fq-diagram-stroke', '--fq-diagram-text']) {
    assert.match(diagramCss, new RegExp(token))
  }
  assert.match(diagramCss, /\.dark[^{]*\{/)
})

test('shared svg assets use theme-friendly colors', () => {
  for (const file of ['architecture-overview.svg', 'execution-model.svg', 'reading-map.svg']) {
    const source = readFileSync(new URL(`../assets/diagrams/${file}`, import.meta.url), 'utf8')
    assert.match(source, /(currentColor|var\(--fq-diagram-)/)
  }
})

test('diagram frame is wired to inline raw svg assets', () => {
  assert.match(diagramFrameSource, /\?raw['"]/)
  assert.match(diagramFrameSource, /v-html=/)
})

test('adopted pages reference shared diagram assets through DiagramFrame props', () => {
  for (const page of adoptedPages) {
    assert.match(page.source, /<DiagramFrame\b[^>]*\b(asset|diagram)=["'][^"']+["'][^>]*>/)
    assert.doesNotMatch(page.source, /<img[^>]+assets\/diagrams\/(?:architecture-overview|execution-model|reading-map)\.svg/)
  }
})
