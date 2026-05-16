import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const navSource = readFileSync(new URL('../.vitepress/theme/content/siteNavigation.ts', import.meta.url), 'utf8')
const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')
const landingPages = [
  '../zh/orientation/index.md',
  '../en/orientation/index.md',
  '../zh/whitepaper/index.md',
  '../en/whitepaper/index.md',
  '../zh/academy/index.md',
  '../en/academy/index.md',
  '../zh/research/index.md',
  '../en/research/index.md',
]

const readDoc = (relativePath) => readFileSync(new URL(relativePath, import.meta.url), 'utf8')

test('top navigation exposes the approved section labels', () => {
  for (const label of ['orientation', 'whitepaper', 'academy', 'reference', 'research']) {
    assert.match(navSource, new RegExp(label, 'i'))
  }
})

test('config wires locale sidebars for the new section roots', () => {
  for (const route of ['/zh/orientation/', '/zh/whitepaper/', '/zh/academy/', '/zh/research/', '/en/orientation/', '/en/whitepaper/', '/en/academy/', '/en/research/']) {
    assert.match(configSource, new RegExp(route.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('research section route config keeps contributing slash-normalized', () => {
  assert.match(configSource, /\[`\/\$\{locale\}\/contributing\/`\]: createResearchSidebar\(locale\)/)
  assert.doesNotMatch(configSource, /\[`\/\$\{locale\}\/contributing`\]: createResearchSidebar\(locale\)/)
  assert.match(configSource, /researchNav: `\^\/\$\{locale\}\/\(research\|performance\|resources\|agents\|archive\|contributing\)\(\/\|\$\)`/)
})

test('academy sidebar stays scoped to academy-owned entry points', () => {
  const academySidebarBlock = navSource.match(/export const academySidebarGroups = \[(.*?)\] as const/s)?.[1] ?? ''
  assert.match(academySidebarBlock, /academyNav/)
  assert.match(academySidebarBlock, /workflows/)

  for (const guideLinkId of ['gettingStarted', 'cliReference', 'configuration', 'deployment']) {
    assert.doesNotMatch(academySidebarBlock, new RegExp(guideLinkId))
    assert.match(navSource, new RegExp(`headingKey: 'guide'[\\s\\S]*${guideLinkId}`))
  }
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
