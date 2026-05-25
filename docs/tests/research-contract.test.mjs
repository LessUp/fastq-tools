import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const readUtf8 = (relativePath) => readFileSync(new URL(relativePath, import.meta.url), 'utf8')

const themeSource = readUtf8('../.vitepress/theme/index.ts')
const referencesSource = readUtf8('../.vitepress/theme/content/references.ts')
const navigationSource = readUtf8('../.vitepress/theme/content/siteNavigation.ts')
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
    academyAlias: readUtf8('../en/academy/index.md'),
    orientationNotice: readUtf8('../en/orientation/index.md'),
    knowledgeMapNotice: readUtf8('../en/knowledge-map/index.md'),
    architecture: readUtf8('../en/architecture/index.md'),
    performance: readUtf8('../en/performance/index.md'),
    reference: readUtf8('../en/reference/index.md'),
    expectedWhitepaperHeadings: ['## Review questions', '## Boundary contract', '## Reading itinerary'],
    expectedAlgorithmsHeadings: ['## Filtering pipeline', '## Statistics path', '## Execution model', '## Memory discipline'],
    expectedArchitectureHeadings: ['## Boundary ledger', '## Control surfaces', '## Failure containment'],
    expectedPerformanceHeadings: ['## Benchmark envelope', '## Threats to validity', '## Reproduction trail'],
    expectedResearchHeadings: ['## Source program', '## Comparative lens', '## Evolution pressure'],
    expectedReferenceHeadings: ['## Implementation lanes', '## Operator reference', '## Contributor surfaces'],
    expectedResearchLinks: ['bibliography', 'related-projects', 'evolution-notes'],
    expectedAliasSignals: ['legacy alias', '../algorithms/'],
    expectedOrientationSignals: ['legacy route', '../whitepaper/', '../reference/'],
    expectedKnowledgeMapSignals: ['legacy route', '../architecture/', '../reference/'],
  },
  {
    locale: 'zh',
    whitepaper: readUtf8('../zh/whitepaper/index.md'),
    algorithms: readUtf8('../zh/algorithms/index.md'),
    researchIndex: readUtf8('../zh/research/index.md'),
    bibliography: readUtf8('../zh/research/bibliography.md'),
    relatedProjects: readUtf8('../zh/research/related-projects.md'),
    evolutionNotes: readUtf8('../zh/research/evolution-notes.md'),
    academyAlias: readUtf8('../zh/academy/index.md'),
    orientationNotice: readUtf8('../zh/orientation/index.md'),
    knowledgeMapNotice: readUtf8('../zh/knowledge-map/index.md'),
    architecture: readUtf8('../zh/architecture/index.md'),
    performance: readUtf8('../zh/performance/index.md'),
    reference: readUtf8('../zh/reference/index.md'),
    expectedWhitepaperHeadings: ['## 评审问题', '## 边界契约', '## 阅读路线'],
    expectedAlgorithmsHeadings: ['## 过滤流水线', '## 统计路径', '## 执行模型', '## 内存约束'],
    expectedArchitectureHeadings: ['## 边界账本', '## 控制界面', '## 故障收敛'],
    expectedPerformanceHeadings: ['## Benchmark 包络', '## 有效性威胁', '## 复现实验路径'],
    expectedResearchHeadings: ['## 来源程序', '## 比较透镜', '## 演进压力'],
    expectedReferenceHeadings: ['## 实施路径', '## 操作参考', '## 贡献者界面'],
    expectedResearchLinks: ['bibliography', 'related-projects', 'evolution-notes'],
    expectedAliasSignals: ['旧别名', '../algorithms/'],
    expectedOrientationSignals: ['旧入口', '../whitepaper/', '../reference/'],
    expectedKnowledgeMapSignals: ['旧入口', '../architecture/', '../reference/'],
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

test('site reference trail keeps the algorithms entry on the maintained algorithms route', () => {
  assert.match(
    referencesSource,
    /id:\s*'workflow-operator-path'[\s\S]*docPath:\s*'algorithms\//,
  )
  assert.doesNotMatch(
    referencesSource,
    /id:\s*'workflow-operator-path'[\s\S]*docPath:\s*'academy\//,
  )
})

test('whitepaper, algorithms, and research layers expose the approved bilingual depth', () => {
  for (const page of bilingualPages) {
    assert.match(page.whitepaper, /ReferenceBadge/)
    assert.match(page.whitepaper, /\.\.\/algorithms\//)
    assert.match(page.whitepaper, /\.\.\/research\//)

    for (const heading of page.expectedWhitepaperHeadings) {
      assert.match(page.whitepaper, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    for (const heading of page.expectedAlgorithmsHeadings) {
      assert.match(page.algorithms, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    for (const heading of page.expectedResearchHeadings) {
      assert.match(page.researchIndex, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    for (const slug of page.expectedResearchLinks) {
      assert.match(page.researchIndex, new RegExp(`\\./${slug}`))
    }

    assert.match(page.bibliography, /<ReferenceList locale="/)
    assert.match(page.relatedProjects, /FastQC|fastp|Cutadapt|seqtk/)
    assert.match(page.evolutionNotes, /RFC-0001|RFC-0003|RFC-0004|RFC-0006/)

    for (const signal of page.expectedAliasSignals) {
      assert.match(page.academyAlias, new RegExp(signal.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    assert.doesNotMatch(page.academyAlias, /<SectionLandingGrid/)
    assert.doesNotMatch(page.orientationNotice, /<SectionLandingGrid/)

    for (const signal of page.expectedOrientationSignals) {
      assert.match(page.orientationNotice, new RegExp(signal.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    for (const signal of page.expectedKnowledgeMapSignals) {
      assert.match(page.knowledgeMapNotice, new RegExp(signal.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }
  }
})

test('architecture, performance, and reference pages align with the expanded whitepaper narrative', () => {
  for (const page of bilingualPages) {
    for (const heading of page.expectedArchitectureHeadings) {
      assert.match(page.architecture, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    for (const heading of page.expectedPerformanceHeadings) {
      assert.match(page.performance, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    for (const heading of page.expectedReferenceHeadings) {
      assert.match(page.reference, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }

    assert.match(page.architecture, /\.\.\/whitepaper\//)
    assert.match(page.architecture, /\.\.\/algorithms\//)
    assert.match(page.performance, /\.\.\/research\//)
    assert.match(page.reference, /\.\.\/research\/bibliography/)
    assert.match(page.reference, /\.\.\/algorithms\//)
  }
})

test('research navigation exposes the new chapter entry points instead of the retired resource hub trio', () => {
  const researchSidebarBlock = navigationSource.match(/export const researchSidebarGroups = \[(.*?)\] as const/s)?.[1] ?? ''

  for (const id of ['bibliography', 'relatedProjects', 'evolutionNotes']) {
    assert.match(researchSidebarBlock, new RegExp(id))
  }

  for (const retiredId of ['benchmarkReport', 'resourcesOverview', 'contributing']) {
    assert.doesNotMatch(researchSidebarBlock, new RegExp(retiredId))
  }
})

test('legacy retirement notices hand readers to maintained section roots only', () => {
  for (const page of bilingualPages) {
    assert.match(page.academyAlias, /\.\.\/algorithms\//)
    assert.match(page.academyAlias, /\.\.\/reference\//)
    assert.match(page.academyAlias, /\.\.\/research\//)
    assert.doesNotMatch(page.academyAlias, /\.\.\/workflows\//)
    assert.doesNotMatch(page.academyAlias, /\.\.\/guide\/getting-started/)
  }
})
