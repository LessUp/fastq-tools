import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const zhHome = readFileSync(new URL('../zh/index.md', import.meta.url), 'utf8')
const enHome = readFileSync(new URL('../en/index.md', import.meta.url), 'utf8')
const heroSource = readFileSync(new URL('../.vitepress/theme/components/SiteHeroPanel.vue', import.meta.url), 'utf8')

test('homepages reference the new narrative components', () => {
  for (const markup of ['<SiteHeroPanel locale="zh" />', '<EvidenceStrip locale="zh" />', '<CitationStrip locale="zh" />', '<SectionLandingGrid locale="zh" section="orientation" />']) {
    assert.match(zhHome, new RegExp(markup.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
  for (const markup of ['<SiteHeroPanel locale="en" />', '<EvidenceStrip locale="en" />', '<CitationStrip locale="en" />', '<SectionLandingGrid locale="en" section="orientation" />']) {
    assert.match(enHome, new RegExp(markup.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('hero copy speaks in whitepaper language', () => {
  assert.match(heroSource, /technical whitepaper|技术白皮书/)
  assert.match(heroSource, /orientation|导读/)
  assert.match(heroSource, /research|研究/)
})
