import test from 'node:test'
import assert from 'node:assert/strict'
import { existsSync, readFileSync } from 'node:fs'

const contentSource = readFileSync(new URL('../.vitepress/theme/content/siteContent.ts', import.meta.url), 'utf8')
const navSource = readFileSync(new URL('../.vitepress/theme/content/siteNavigation.ts', import.meta.url), 'utf8')
const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')
const landingGridSource = readFileSync(new URL('../.vitepress/theme/components/SectionLandingGrid.vue', import.meta.url), 'utf8')
const heroPanelSource = readFileSync(new URL('../.vitepress/theme/components/SiteHeroPanel.vue', import.meta.url), 'utf8')
const pillarGridSource = readFileSync(new URL('../.vitepress/theme/components/PillarGrid.vue', import.meta.url), 'utf8')
const workflowPathsSource = readFileSync(new URL('../.vitepress/theme/components/WorkflowPaths.vue', import.meta.url), 'utf8')
const readingTracksSource = readFileSync(new URL('../.vitepress/theme/components/ReadingTracks.vue', import.meta.url), 'utf8')
const researchRailSource = readFileSync(new URL('../.vitepress/theme/components/ResearchRail.vue', import.meta.url), 'utf8')
const landingPages = [
  '../zh/orientation/index.md',
  '../en/orientation/index.md',
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
  for (const label of ['orientation', 'whitepaper', 'algorithms', 'reference', 'research']) {
    assert.match(navSource, new RegExp(label, 'i'))
  }
})

test('config wires locale sidebars for the new section roots', () => {
  for (const route of ['/zh/orientation/', '/zh/whitepaper/', '/zh/algorithms/', '/zh/reference/', '/zh/research/', '/en/orientation/', '/en/whitepaper/', '/en/algorithms/', '/en/reference/', '/en/research/']) {
    assert.match(configSource, new RegExp(route.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('algorithms foundation stays canonical on algorithms routes', () => {
  assert.match(contentSource, /algorithms:\s*\{[\s\S]*path: 'algorithms\/'/)
  assert.doesNotMatch(contentSource, /algorithms:\s*\{[\s\S]*path: 'academy\/'/)
  assert.match(contentSource, /algorithms:\s*\{[\s\S]*aliases: \['academy\/', 'workflows\/'\]/)
  assert.match(configSource, /algorithms:\s*'\/zh\/algorithms\/'/)
  assert.match(configSource, /algorithms:\s*'\/en\/algorithms\/'/)
  assert.doesNotMatch(configSource, /algorithms:\s*'\/zh\/academy\/'/)
  assert.doesNotMatch(configSource, /algorithms:\s*'\/en\/academy\/'/)
  assert.match(configSource, /algorithms: `\^\/\$\{locale\}\/\(algorithms\|academy\|workflows\)\(\/\|\$\)`/)
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

test('shared landing and homepage copy treat academy as a legacy alias and foreground research chapters', () => {
  assert.match(landingGridSource, /Academy \(legacy alias\)/)
  assert.match(landingGridSource, /学院（旧别名）/)
  assert.match(landingGridSource, /linkId: 'algorithms'/)
  assert.match(landingGridSource, /linkId: 'bibliography'/)
  assert.match(landingGridSource, /linkId: 'relatedProjects'/)
  assert.match(landingGridSource, /linkId: 'evolutionNotes'/)
  assert.doesNotMatch(landingGridSource, /linkId: 'benchmarkReport'/)
  assert.doesNotMatch(landingGridSource, /linkId: 'resourcesOverview'/)
  assert.doesNotMatch(landingGridSource, /linkId: 'contributing'/)

  assert.doesNotMatch(heroPanelSource, /academy guidance|学院或研究证据/)
  assert.doesNotMatch(pillarGridSource, /Academy cards|学院卡片/)
  assert.doesNotMatch(workflowPathsSource, /Academy exposes|学院层把/)
  assert.match(readingTracksSource, /researchNav', 'bibliography', 'evolutionNotes'/)
  assert.match(researchRailSource, /linkId: 'bibliography'/)
  assert.match(researchRailSource, /linkId: 'relatedProjects'/)
  assert.match(researchRailSource, /linkId: 'evolutionNotes'/)
  assert.doesNotMatch(researchRailSource, /linkId: 'resourcesOverview'/)
  assert.doesNotMatch(researchRailSource, /linkId: 'contributing'/)
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
