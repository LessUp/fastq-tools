import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const readUtf8 = (relativePath) => readFileSync(new URL(relativePath, import.meta.url), 'utf8')

const themeSource = readUtf8('../.vitepress/theme/index.ts')
const packageJson = readUtf8('../package.json')

const bilingualPages = [
  {
    locale: 'en',
    whitepaper: readUtf8('../en/whitepaper/index.md'),
    algorithms: readUtf8('../en/algorithms/index.md'),
    researchIndex: readUtf8('../en/research/index.md'),
    bibliography: readUtf8('../en/research/bibliography.md'),
    relatedProjects: readUtf8('../en/research/related-projects.md'),
    evolutionNotes: readUtf8('../en/research/evolution-notes.md'),
    architecture: readUtf8('../en/architecture/index.md'),
    performance: readUtf8('../en/performance/index.md'),
    reference: readUtf8('../en/reference/index.md'),
    expectedAlgorithmsHeadings: ['## Filtering pipeline', '## Statistics path', '## Execution model', '## Memory discipline'],
    expectedResearchLinks: ['bibliography', 'related-projects', 'evolution-notes'],
  },
  {
    locale: 'zh',
    whitepaper: readUtf8('../zh/whitepaper/index.md'),
    algorithms: readUtf8('../zh/algorithms/index.md'),
    researchIndex: readUtf8('../zh/research/index.md'),
    bibliography: readUtf8('../zh/research/bibliography.md'),
    relatedProjects: readUtf8('../zh/research/related-projects.md'),
    evolutionNotes: readUtf8('../zh/research/evolution-notes.md'),
    architecture: readUtf8('../zh/architecture/index.md'),
    performance: readUtf8('../zh/performance/index.md'),
    reference: readUtf8('../zh/reference/index.md'),
    expectedAlgorithmsHeadings: ['## 过滤流水线', '## 统计路径', '## 执行模型', '## 内存约束'],
    expectedResearchLinks: ['bibliography', 'related-projects', 'evolution-notes'],
  },
]

test('npm test includes the deep-content research contract', () => {
  assert.match(packageJson, /research-contract\.test\.mjs/)
})

test('theme registers the reusable reference components', () => {
  for (const componentName of ['ReferenceList', 'ReferenceBadge']) {
    assert.match(themeSource, new RegExp(`import ${componentName} from ['"]./components/${componentName}\\.vue['"]`))
    assert.match(themeSource, new RegExp(`component\\(['"]${componentName}['"]`))
  }
})

test('whitepaper, algorithms, and research layers expose the approved bilingual depth', () => {
  for (const page of bilingualPages) {
    assert.match(page.whitepaper, /ReferenceBadge/)
    assert.match(page.whitepaper, /\.\.\/algorithms\//)
    assert.match(page.whitepaper, /\.\.\/research\//)

    for (const heading of page.expectedAlgorithmsHeadings) {
      assert.match(page.algorithms, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    for (const slug of page.expectedResearchLinks) {
      assert.match(page.researchIndex, new RegExp(`\\./${slug}`))
    }

    assert.match(page.bibliography, /<ReferenceList locale="/)
    assert.match(page.relatedProjects, /FastQC|fastp|Cutadapt|seqtk/)
    assert.match(page.evolutionNotes, /RFC-0001|RFC-0003|RFC-0004|RFC-0006/)
  }
})

test('architecture, performance, and reference pages align with the expanded whitepaper narrative', () => {
  for (const page of bilingualPages) {
    assert.match(page.architecture, /\.\.\/whitepaper\//)
    assert.match(page.architecture, /\.\.\/algorithms\//)
    assert.match(page.performance, /\.\.\/research\//)
    assert.match(page.reference, /\.\.\/research\/bibliography/)
    assert.match(page.reference, /\.\.\/algorithms\//)
  }
})
