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
  assert.match(diagramCss, /\.dark[^{]*\{[^}]*--fq-diagram-stroke:\s*var\(--vp-c-brand-2\);/s)
})

test('shared svg assets use theme-friendly colors', () => {
  for (const file of ['architecture-overview.svg', 'execution-model.svg', 'reading-map.svg']) {
    const source = readFileSync(new URL(`../assets/diagrams/${file}`, import.meta.url), 'utf8')
    assert.doesNotMatch(source, /@media/)
    assert.doesNotMatch(source, /prefers-color-scheme/)
    assert.doesNotMatch(source, /:root\s*\{/)
    assert.match(source, /(currentColor|var\(--fq-diagram-)/)
  }
})

test('diagram frame is wired to inline raw svg assets', () => {
  assert.match(diagramFrameSource, /\?raw['"]/)
  assert.match(diagramFrameSource, /v-html=/)
})

test('diagram frame renders a caption when the prop or named slot is present', () => {
  assert.match(diagramFrameSource, /const hasCaptionSlot = computed\(\(\) => Boolean\(slots\.caption\)\)/)
  assert.match(diagramFrameSource, /<figcaption v-if="caption \|\| hasCaptionSlot">/)
})

test('adopted pages reference shared diagram assets through DiagramFrame props', () => {
  for (const page of adoptedPages) {
    assert.match(page.source, /<DiagramFrame\b[^>]*\basset=["'][^"']+["'][^>]*>/)
    assert.doesNotMatch(page.source, /<img[^>]+assets\/diagrams\/(?:architecture-overview|execution-model|reading-map)\.svg/)
  }
})
