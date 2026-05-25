import test from 'node:test'
import assert from 'node:assert/strict'
import { existsSync, readFileSync } from 'node:fs'

const contentSource = readFileSync(new URL('../.vitepress/theme/content/siteContent.ts', import.meta.url), 'utf8')
const navSource = readFileSync(new URL('../.vitepress/theme/content/siteNavigation.ts', import.meta.url), 'utf8')
const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')
const enResourcesSource = readFileSync(new URL('../en/resources/index.md', import.meta.url), 'utf8')
const zhResourcesSource = readFileSync(new URL('../zh/resources/index.md', import.meta.url), 'utf8')
const landingGridSource = readFileSync(new URL('../.vitepress/theme/components/SectionLandingGrid.vue', import.meta.url), 'utf8')
const heroPanelSource = readFileSync(new URL('../.vitepress/theme/components/SiteHeroPanel.vue', import.meta.url), 'utf8')
const whitepaperHeroSource = readFileSync(new URL('../.vitepress/theme/components/WhitepaperHero.vue', import.meta.url), 'utf8')
const pillarGridSource = readFileSync(new URL('../.vitepress/theme/components/PillarGrid.vue', import.meta.url), 'utf8')
const workflowPathsSource = readFileSync(new URL('../.vitepress/theme/components/WorkflowPaths.vue', import.meta.url), 'utf8')
const readingTracksSource = readFileSync(new URL('../.vitepress/theme/components/ReadingTracks.vue', import.meta.url), 'utf8')
const researchRailSource = readFileSync(new URL('../.vitepress/theme/components/ResearchRail.vue', import.meta.url), 'utf8')
const readingLanesSource = readFileSync(new URL('../.vitepress/theme/components/ReadingLanes.vue', import.meta.url), 'utf8')
const enOrientationSource = readFileSync(new URL('../en/orientation/index.md', import.meta.url), 'utf8')
const zhOrientationSource = readFileSync(new URL('../zh/orientation/index.md', import.meta.url), 'utf8')
const enKnowledgeMapSource = readFileSync(new URL('../en/knowledge-map/index.md', import.meta.url), 'utf8')
const zhKnowledgeMapSource = readFileSync(new URL('../zh/knowledge-map/index.md', import.meta.url), 'utf8')
const enAcademySource = readFileSync(new URL('../en/academy/index.md', import.meta.url), 'utf8')
const zhAcademySource = readFileSync(new URL('../zh/academy/index.md', import.meta.url), 'utf8')
const enWhySource = readFileSync(new URL('../en/why-fastqtools/index.md', import.meta.url), 'utf8')
const zhWhySource = readFileSync(new URL('../zh/why-fastqtools/index.md', import.meta.url), 'utf8')
const landingPages = [
  '../zh/whitepaper/index.md',
  '../en/whitepaper/index.md',
  '../zh/algorithms/index.md',
  '../en/algorithms/index.md',
  '../zh/reference/index.md',
  '../en/reference/index.md',
  '../zh/research/index.md',
  '../en/research/index.md',
]

const readDoc = (relativePath) => readFileSync(new URL(relativePath, import.meta.url), 'utf8')

test('top navigation exposes the approved section labels', () => {
  const topNavBlock = navSource.match(/export const topNavLinkIds = \[(.*?)\] as const/s)?.[1] ?? ''

  for (const label of ['whitepaper', 'architecture', 'algorithms', 'performance', 'referenceNav', 'researchNav']) {
    assert.match(topNavBlock, new RegExp(label, 'i'))
  }

  assert.doesNotMatch(topNavBlock, /orientationNav/)
  assert.doesNotMatch(topNavBlock, /academy/)
  assert.doesNotMatch(topNavBlock, /knowledgeMap/)
})

test('shared navigation data drops retired section helper models', () => {
  assert.doesNotMatch(navSource, /orientationNav:/)
  assert.doesNotMatch(navSource, /knowledgeMap:/)
  assert.doesNotMatch(navSource, /orientationSidebarGroups/)
  assert.doesNotMatch(navSource, /knowledgeMapSections/)
  assert.doesNotMatch(contentSource, /orientation:\s*'导读'/)
  assert.doesNotMatch(contentSource, /orientation:\s*'Orientation'/)
  assert.doesNotMatch(contentSource, /academy:\s*'算法（学院旧别名）'/)
  assert.doesNotMatch(contentSource, /academy:\s*'Algorithms \(legacy Academy alias\)'/)
  assert.doesNotMatch(contentSource, /knowledgeMap:\s*'知识地图'/)
  assert.doesNotMatch(contentSource, /knowledgeMap:\s*'Knowledge Map'/)
})

test('config wires locale sidebars only for maintained section roots', () => {
  for (const route of ['/zh/whitepaper/', '/zh/architecture/', '/zh/algorithms/', '/zh/performance/', '/zh/reference/', '/zh/research/', '/en/whitepaper/', '/en/architecture/', '/en/algorithms/', '/en/performance/', '/en/reference/', '/en/research/']) {
    assert.match(configSource, new RegExp(route.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const retiredRoute of ['/zh/orientation/', '/en/orientation/', '/${locale}/academy/', '/${locale}/knowledge-map/']) {
    assert.doesNotMatch(configSource, new RegExp(retiredRoute.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('core section foundations stay canonical on maintained routes', () => {
  assert.match(contentSource, /algorithms:\s*\{[\s\S]*path: 'algorithms\/'/)
  assert.doesNotMatch(contentSource, /algorithms:\s*\{[\s\S]*path: 'academy\/'/)
  assert.match(contentSource, /algorithms:\s*\{[\s\S]*aliases: \['workflows\/'\]/)
  assert.match(contentSource, /whitepaper:\s*\{[\s\S]*aliases: \['why-fastqtools\/'\]/)
  assert.doesNotMatch(contentSource, /whitepaper:\s*\{[\s\S]*aliases: \[[\s\S]*orientation\//)
  assert.doesNotMatch(contentSource, /architecture:\s*\{[\s\S]*aliases: \[[\s\S]*knowledge-map\//)
  assert.match(configSource, /algorithms:\s*'\/zh\/algorithms\/'/)
  assert.match(configSource, /algorithms:\s*'\/en\/algorithms\/'/)
  assert.doesNotMatch(configSource, /algorithms:\s*'\/zh\/academy\/'/)
  assert.doesNotMatch(configSource, /algorithms:\s*'\/en\/academy\/'/)
  assert.match(configSource, /algorithms: `\^\/\$\{locale\}\/\(algorithms\|workflows\)\(\/\|\$\)`/)
  assert.match(configSource, /whitepaper: `\^\/\$\{locale\}\/\(\$\|\(whitepaper\|why-fastqtools\)\(\/\|\$\)\)`/)
  assert.match(configSource, /architecture: `\^\/\$\{locale\}\/architecture\(\/\|\$\)`/)
})

test('research section route config keeps contributing slash-normalized', () => {
  assert.match(configSource, /\[`\/\$\{locale\}\/contributing\/`\]: createResearchSidebar\(locale\)/)
  assert.doesNotMatch(configSource, /\[`\/\$\{locale\}\/contributing`\]: createResearchSidebar\(locale\)/)
  assert.match(configSource, /researchNav: `\^\/\$\{locale\}\/\(research\|resources\|agents\|archive\|contributing\)\(\/\|\$\)`/)
  assert.doesNotMatch(configSource, /researchNav: `\^\/\$\{locale\}\/\(research\|performance\|resources\|agents\|archive\|contributing\)\(\/\|\$\)`/)
  assert.match(navSource, /contributing:\s*\{\s*path: 'contributing\/'/)
  assert.doesNotMatch(navSource, /contributing:\s*\{\s*path: 'contributing'/)
})

test('contributing pages use directory index routes for GitHub Pages clean URLs', () => {
  for (const nestedPath of ['../en/contributing/index.md', '../zh/contributing/index.md']) {
    assert.equal(existsSync(new URL(nestedPath, import.meta.url)), true, `${nestedPath} should exist`)
  }

  for (const flatPath of ['../en/contributing.md', '../zh/contributing.md']) {
    assert.equal(existsSync(new URL(flatPath, import.meta.url)), false, `${flatPath} should be replaced`)
  }
})

test('algorithms sidebar stays scoped to algorithms-owned entry points', () => {
  const algorithmsSidebarBlock = navSource.match(/export const algorithmsSidebarGroups = \[(.*?)\] as const/s)?.[1] ?? ''
  assert.match(algorithmsSidebarBlock, /algorithms/)
  assert.match(algorithmsSidebarBlock, /workflows/)
  assert.doesNotMatch(algorithmsSidebarBlock, /academyNav/)

  assert.match(algorithmsSidebarBlock, /gettingStarted/)
  assert.match(algorithmsSidebarBlock, /cliReference/)
})

test('legacy IA routes are lightweight retirement notices instead of maintained hubs', () => {
  for (const legacyPage of [
    enOrientationSource,
    zhOrientationSource,
    enKnowledgeMapSource,
    zhKnowledgeMapSource,
    enAcademySource,
    zhAcademySource,
  ]) {
    assert.doesNotMatch(legacyPage, /<SectionLandingGrid/)
  }

  assert.match(enOrientationSource, /legacy route|retired/i)
  assert.match(zhOrientationSource, /旧入口|已退役/)
  assert.match(enKnowledgeMapSource, /legacy route|retired/i)
  assert.match(zhKnowledgeMapSource, /旧入口|已退役/)
  assert.match(enAcademySource, /legacy alias|retired/i)
  assert.match(zhAcademySource, /旧别名|已退役/)
})

test('research sidebar excludes release note changelog links', () => {
  const researchSidebarBlock = navSource.match(/export const researchSidebarGroups = \[(.*?)\] as const/s)?.[1] ?? ''
  assert.match(researchSidebarBlock, /bibliography/)
  assert.match(researchSidebarBlock, /relatedProjects/)
  assert.match(researchSidebarBlock, /evolutionNotes/)
  assert.doesNotMatch(researchSidebarBlock, /benchmarkReport/)
  assert.doesNotMatch(researchSidebarBlock, /resourcesOverview/)
  assert.doesNotMatch(researchSidebarBlock, /contributing/)
  assert.doesNotMatch(researchSidebarBlock, /changelog/)
})

test('reference navigation no longer publishes changelog routes', () => {
  assert.doesNotMatch(navSource, /changelog:\s*\{/)
  assert.doesNotMatch(navSource, /headingKey:\s*'releaseNotes'/)
  assert.doesNotMatch(configSource, /\[`\/\$\{locale\}\/release-notes\/`\]: createReferenceSidebar\(locale\)/)
  assert.doesNotMatch(configSource, /referenceNav: `\^\/\$\{locale\}\/\(reference\|guide\|api\|dev\|release-notes\)\//)
  assert.doesNotMatch(contentSource, /changelog:\s*'变更记录'/)
  assert.doesNotMatch(contentSource, /changelog:\s*'Changelog'/)
  assert.doesNotMatch(contentSource, /aliases: \['guide\/', 'api\/', 'dev\/', 'release-notes\/'\]/)
})

test('resource hubs point readers to releases and root changelog instead of docs changelog pages', () => {
  for (const source of [enResourcesSource, zhResourcesSource]) {
    assert.doesNotMatch(source, /\.\.\/release-notes\/changelog/)
    assert.match(source, /https:\/\/github\.com\/LessUp\/fastq-tools\/releases/)
    assert.match(source, /https:\/\/github\.com\/LessUp\/fastq-tools\/blob\/master\/CHANGELOG\.md/)
  }
})

test('shared landing and homepage copy treat academy as a legacy alias and foreground research chapters', () => {
  assert.doesNotMatch(landingGridSource, /Academy \(legacy alias\)/)
  assert.doesNotMatch(landingGridSource, /学院（旧别名）/)
  assert.doesNotMatch(landingGridSource, /orientationNav|knowledgeMap/)
  assert.match(landingGridSource, /type SectionKey = 'whitepaper' \| 'reference' \| 'research'/)
  assert.match(landingGridSource, /linkId: 'bibliography'/)
  assert.match(landingGridSource, /linkId: 'relatedProjects'/)
  assert.match(landingGridSource, /linkId: 'evolutionNotes'/)
  assert.doesNotMatch(landingGridSource, /linkId: 'benchmarkReport'/)
  assert.doesNotMatch(landingGridSource, /linkId: 'resourcesOverview'/)
  assert.doesNotMatch(landingGridSource, /linkId: 'contributing'/)

  assert.doesNotMatch(heroPanelSource, /academy guidance|学院或研究证据/)
  assert.doesNotMatch(heroPanelSource, /orientationNav|Start with orientation|从导读开始/)
  assert.doesNotMatch(whitepaperHeroSource, /orientationNav|Start with orientation|从导读开始/)
  assert.doesNotMatch(pillarGridSource, /Academy cards|学院卡片/)
  assert.doesNotMatch(workflowPathsSource, /Academy exposes|学院层把/)
  assert.doesNotMatch(readingTracksSource, /orientationNav/)
  assert.match(readingTracksSource, /whitepaper', 'performance', 'referenceNav'/)
  assert.match(readingTracksSource, /researchNav', 'bibliography', 'evolutionNotes'/)
  assert.match(researchRailSource, /linkId: 'bibliography'/)
  assert.match(researchRailSource, /linkId: 'relatedProjects'/)
  assert.match(researchRailSource, /linkId: 'evolutionNotes'/)
  assert.doesNotMatch(researchRailSource, /linkId: 'resourcesOverview'/)
  assert.doesNotMatch(researchRailSource, /linkId: 'contributing'/)
})

test('theme only registers maintained landing components', () => {
  const themeSource = readFileSync(new URL('../.vitepress/theme/index.ts', import.meta.url), 'utf8')

  assert.doesNotMatch(themeSource, /SiteHeroPanel/)
  assert.doesNotMatch(themeSource, /WhitepaperHero/)
  assert.doesNotMatch(themeSource, /ReadingTracks/)
  assert.doesNotMatch(themeSource, /KnowledgeMap/)
})

test('homepage and why-fastqtools copy route readers only through maintained chapters', () => {
  assert.match(readingLanesSource, /steps: \['whitepaperNav', 'why', 'performance'\]/)
  assert.doesNotMatch(readingLanesSource, /steps: \['orientationNav', 'whitepaperNav', 'performance'\]/)

  for (const source of [enWhySource, zhWhySource]) {
    assert.match(source, /\.\.\/whitepaper\//)
    assert.match(source, /\.\.\/reference\//)
    assert.doesNotMatch(source, /\.\.\/orientation\//)
    assert.doesNotMatch(source, /\.\.\/knowledge-map\//)
  }
})

test('section landing pages include frontmatter and non-empty body copy', () => {
  for (const page of landingPages) {
    const source = readDoc(page)
    const frontmatter = source.match(/^---\n([\s\S]*?)\n---\n?/)?.[1] ?? ''
    assert.notEqual(frontmatter, '', `${page} should define frontmatter`)
    assert.match(frontmatter, /^title:\s*.+$/m, page)
    assert.match(frontmatter, /^description:\s*.+$/m, page)

    const body = source
      .replace(/^---\n[\s\S]*?\n---\n?/, '')
      .replace(/<SectionLandingGrid[^>]*\/>\n?/g, '')
      .trim()

    assert.notEqual(body, '', `${page} should not be blank after the landing grid`)
  }
})
