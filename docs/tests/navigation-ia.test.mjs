import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const contentSource = readFileSync(new URL('../.vitepress/theme/content/siteContent.ts', import.meta.url), 'utf8')
const navSource = readFileSync(new URL('../.vitepress/theme/content/siteNavigation.ts', import.meta.url), 'utf8')
const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')
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
  assert.match(configSource, /algorithms:\s*'\/zh\/algorithms\/'/)
  assert.match(configSource, /algorithms:\s*'\/en\/algorithms\/'/)
  assert.doesNotMatch(configSource, /algorithms:\s*'\/zh\/academy\/'/)
  assert.doesNotMatch(configSource, /algorithms:\s*'\/en\/academy\/'/)
  assert.match(configSource, /algorithms: `\^\/\$\{locale\}\/\(algorithms\|workflows\)\(\/\|\$\)`/)
  assert.doesNotMatch(configSource, /algorithms: `\^\/\$\{locale\}\/\(academy\|workflows\)\(\/\|\$\)`/)
})

test('research section route config keeps contributing slash-normalized', () => {
  assert.match(configSource, /\[`\/\$\{locale\}\/contributing\/`\]: createResearchSidebar\(locale\)/)
  assert.doesNotMatch(configSource, /\[`\/\$\{locale\}\/contributing`\]: createResearchSidebar\(locale\)/)
  assert.match(configSource, /researchNav: `\^\/\$\{locale\}\/\(research\|resources\|agents\|archive\|contributing\)\(\/\|\$\)`/)
  assert.doesNotMatch(configSource, /researchNav: `\^\/\$\{locale\}\/\(research\|performance\|resources\|agents\|archive\|contributing\)\(\/\|\$\)`/)
  assert.match(navSource, /contributing:\s*\{\s*path: 'contributing\/'/)
  assert.doesNotMatch(navSource, /contributing:\s*\{\s*path: 'contributing'/)
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
  assert.doesNotMatch(researchSidebarBlock, /changelog/)
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
