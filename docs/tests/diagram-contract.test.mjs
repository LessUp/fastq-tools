import test from 'node:test'
import assert from 'node:assert/strict'
import { existsSync, readFileSync } from 'node:fs'

const diagramCss = readFileSync(new URL('../.vitepress/theme/styles/diagrams.css', import.meta.url), 'utf8')
const diagramFrameSource = readFileSync(
  new URL('../.vitepress/theme/components/DiagramFrame.vue', import.meta.url),
  'utf8',
)
const readUtf8 = (relativePath) => readFileSync(new URL(relativePath, import.meta.url), 'utf8')
const exists = (relativePath) => existsSync(new URL(relativePath, import.meta.url))

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
  for (const token of [
    '--fq-diagram-bg',
    '--fq-diagram-layer',
    '--fq-diagram-accent',
    '--fq-diagram-signal',
    '--fq-diagram-stroke',
    '--fq-diagram-text',
    '--fq-diagram-muted',
  ]) {
    assert.match(diagramCss, new RegExp(token))
  }
  assert.match(diagramCss, /\.dark[^{]*\{/)
  assert.match(diagramCss, /\.dark[^{]*\{[^}]*--fq-diagram-accent:/s)
})

test('diagram frame dispatches theme-native vue diagrams instead of raw svg injection', () => {
  for (const componentName of ['ArchitectureOverviewDiagram', 'ExecutionModelDiagram', 'ReadingMapDiagram']) {
    assert.match(diagramFrameSource, new RegExp(componentName))
  }
  assert.match(diagramFrameSource, /locale\?: 'en' \| 'zh'/)
  assert.doesNotMatch(diagramFrameSource, /\?raw['"]/)
  assert.doesNotMatch(diagramFrameSource, /v-html=/)
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

test('all publication diagrams are inline vue components with theme tokens and wrapped labels', () => {
  const sources = [
    '../.vitepress/theme/components/diagrams/ArchitectureOverviewDiagram.vue',
    '../.vitepress/theme/components/diagrams/ExecutionModelDiagram.vue',
    '../.vitepress/theme/components/diagrams/ReadingMapDiagram.vue',
  ]

  for (const relativePath of sources) {
    assert.equal(exists(relativePath), true, `${relativePath} should exist`)
    const source = readUtf8(relativePath)
    assert.match(source, /<svg[\s\S]*viewBox=/)
    assert.match(source, /var\(--fq-diagram-/)
    assert.match(source, /<tspan\b/)
    assert.doesNotMatch(source, /#0[0-9a-f]{2,7}/i)
  }
})

test('shared diagrams keep locale-aware labels on bilingual narrative pages', () => {
  for (const page of adoptedPages) {
    const expectedLocale = page.relativePath.includes('/zh/') ? 'zh' : 'en'
    assert.match(page.source, new RegExp(`locale=["']${expectedLocale}["']`))
  }

  assert.match(readUtf8('../.vitepress/theme/components/diagrams/ArchitectureOverviewDiagram.vue'), /FastQTools 架构总览/)
  assert.match(readUtf8('../.vitepress/theme/components/diagrams/ExecutionModelDiagram.vue'), /FastQTools 执行模型/)
  assert.match(readUtf8('../.vitepress/theme/components/diagrams/ReadingMapDiagram.vue'), /FastQTools 阅读地图/)
})
