import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const zhHome = readFileSync(new URL('../zh/index.md', import.meta.url), 'utf8')
const enHome = readFileSync(new URL('../en/index.md', import.meta.url), 'utf8')
const heroSource = readFileSync(new URL('../.vitepress/theme/components/SiteHeroPanel.vue', import.meta.url), 'utf8')

const homepageContracts = [
  {
    locale: 'zh',
    source: zhHome,
    expectedMarkup: [
      '<SiteHeroPanel locale="zh" />',
      '<EvidenceStrip locale="zh" />',
      '<CitationStrip locale="zh" />',
      '<SectionLandingGrid locale="zh" section="orientation" />',
      '<SectionLandingGrid locale="zh" section="whitepaper" />',
      '<SectionLandingGrid locale="zh" section="academy" />',
      '<SectionLandingGrid locale="zh" section="research" />',
    ],
  },
  {
    locale: 'en',
    source: enHome,
    expectedMarkup: [
      '<SiteHeroPanel locale="en" />',
      '<EvidenceStrip locale="en" />',
      '<CitationStrip locale="en" />',
      '<SectionLandingGrid locale="en" section="orientation" />',
      '<SectionLandingGrid locale="en" section="whitepaper" />',
      '<SectionLandingGrid locale="en" section="academy" />',
      '<SectionLandingGrid locale="en" section="research" />',
    ],
  },
]

test('homepages reference the new narrative components', () => {
  for (const { locale, source, expectedMarkup } of homepageContracts) {
    for (const markup of expectedMarkup) {
      assert.match(source, new RegExp(markup.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')), `${locale} homepage should include ${markup}`)
    }
  }
})

test('hero copy speaks in whitepaper language', () => {
  assert.match(heroSource, /technical whitepaper|技术白皮书/)
  assert.match(heroSource, /orientation|导读/)
  assert.match(heroSource, /research|研究/)
})
