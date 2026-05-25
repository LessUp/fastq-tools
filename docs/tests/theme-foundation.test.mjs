import test from 'node:test'
import assert from 'node:assert/strict'
import { existsSync, readFileSync } from 'node:fs'

const packageJson = JSON.parse(readFileSync(new URL('../package.json', import.meta.url), 'utf8'))
const navSource = readFileSync(new URL('../.vitepress/theme/content/siteNavigation.ts', import.meta.url), 'utf8')
const contentSource = readFileSync(new URL('../.vitepress/theme/content/siteContent.ts', import.meta.url), 'utf8')
const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')
const tokensSource = readFileSync(new URL('../.vitepress/theme/styles/tokens.css', import.meta.url), 'utf8')
const baseStyles = readFileSync(new URL('../.vitepress/theme/styles/base.css', import.meta.url), 'utf8')
const themeStyles = readFileSync(new URL('../.vitepress/theme/style.css', import.meta.url), 'utf8')

const getActiveMatchPattern = (id, locale = 'en') => {
  const escapedId = id.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')
  const rawPattern = configSource.match(new RegExp(escapedId + ':\\s*`([^`]+)`'))?.[1]

  assert.notEqual(rawPattern, undefined, `missing activeMatch pattern for ${id}`)

  return new RegExp(rawPattern.replaceAll('${locale}', locale))
}

test('docs package exposes the theme foundation test through npm test', () => {
  assert.match(packageJson.scripts.test, /theme-foundation\.test\.mjs/)
})

test('theme foundation promotes the whitepaper-first IA at the top level', () => {
  const topLevelBlock = navSource.match(/topNavLinkIds\s*=\s*\[(.*?)\]\s*as const/s)?.[1] ?? ''

  for (const id of ['whitepaper', 'architecture', 'algorithms', 'performance', 'referenceNav', 'researchNav']) {
    assert.match(topLevelBlock, new RegExp(`['"]${id}['"]`), `missing ${id} in top navigation`)
  }

  assert.doesNotMatch(topLevelBlock, /orientationNav/)
  assert.doesNotMatch(topLevelBlock, /academyNav/)
})

test('theme foundation ships dedicated shared content and reference data modules', () => {
  for (const relativePath of [
    '../.vitepress/theme/content/siteContent.ts',
    '../.vitepress/theme/content/references.ts',
  ]) {
    assert.equal(existsSync(new URL(relativePath, import.meta.url)), true, `${relativePath} should exist`)
  }
})

test('theme foundation removes the retired KnowledgeMap component file', () => {
  assert.equal(
    existsSync(new URL('../.vitepress/theme/components/KnowledgeMap.vue', import.meta.url)),
    false,
    'KnowledgeMap.vue should be removed once the component is retired',
  )
})

test('config active matches pivot around the new IA concepts', () => {
  for (const token of ['whitepaper:', 'architecture:', 'algorithms:', 'performance:', 'referenceNav:', 'researchNav:']) {
    assert.match(configSource, new RegExp(token))
  }

  assert.doesNotMatch(configSource, /orientationNav:/)
  assert.doesNotMatch(configSource, /academyNav:/)
})

test('theme foundation keeps only maintained section concepts in shared content', () => {
  for (const concept of ['whitepaper', 'architecture', 'algorithms', 'performance', 'referenceNav', 'researchNav']) {
    assert.match(contentSource, new RegExp(concept))
  }

  assert.doesNotMatch(contentSource, /orientation:\s*'导读'/)
  assert.doesNotMatch(contentSource, /orientation:\s*'Orientation'/)
  assert.doesNotMatch(contentSource, /academy:\s*'算法（学院旧别名）'/)
  assert.doesNotMatch(contentSource, /academy:\s*'Algorithms \(legacy Academy alias\)'/)
})

test('top-level active matches keep performance and research nav states mutually exclusive', () => {
  const performanceActiveMatch = getActiveMatchPattern('performance')
  const researchActiveMatch = getActiveMatchPattern('researchNav')

  assert.match('/en/performance/', performanceActiveMatch)
  assert.doesNotMatch('/en/performance/', researchActiveMatch)

  assert.match('/en/research/', researchActiveMatch)
  assert.match('/en/resources/', researchActiveMatch)
  assert.doesNotMatch('/en/research/', performanceActiveMatch)
})

test('whitepaper nav stays active on locale homepages and maintained whitepaper pages only', () => {
  const enWhitepaperActiveMatch = getActiveMatchPattern('whitepaper', 'en')
  const zhWhitepaperActiveMatch = getActiveMatchPattern('whitepaper', 'zh')

  for (const route of ['/en/', '/en/whitepaper/', '/en/why-fastqtools/']) {
    assert.match(route, enWhitepaperActiveMatch)
  }

  for (const route of ['/zh/', '/zh/whitepaper/', '/zh/why-fastqtools/']) {
    assert.match(route, zhWhitepaperActiveMatch)
  }

  assert.doesNotMatch('/en/orientation/', enWhitepaperActiveMatch)
  assert.doesNotMatch('/zh/orientation/', zhWhitepaperActiveMatch)
  assert.doesNotMatch('/en/architecture/', enWhitepaperActiveMatch)
  assert.doesNotMatch('/zh/algorithms/', zhWhitepaperActiveMatch)
})

test('retired route aliases do not participate in maintained section highlighting', () => {
  const enArchitectureActiveMatch = getActiveMatchPattern('architecture', 'en')
  const enAlgorithmsActiveMatch = getActiveMatchPattern('algorithms', 'en')

  assert.doesNotMatch('/en/knowledge-map/', enArchitectureActiveMatch)
  assert.doesNotMatch('/en/academy/', enAlgorithmsActiveMatch)
})

test('theme foundation keeps retired routes outside maintained alias lists', () => {
  assert.match(contentSource, /whitepaper:\s*\{[\s\S]*path: 'whitepaper\/'/)
  assert.match(contentSource, /architecture:\s*\{[\s\S]*path: 'architecture\/'/)
  assert.match(contentSource, /algorithms:\s*\{[\s\S]*path: 'algorithms\/'/)

  assert.doesNotMatch(contentSource, /whitepaper:\s*\{[\s\S]*aliases: \[[\s\S]*orientation\//)
  assert.doesNotMatch(contentSource, /architecture:\s*\{[\s\S]*aliases: \[[\s\S]*knowledge-map\//)
  assert.doesNotMatch(contentSource, /algorithms:\s*\{[\s\S]*aliases: \[[\s\S]*academy\//)
})

test('publication shell widens the homepage and doc reading surface', () => {
  assert.match(baseStyles, /\.VPDoc\s+\.container\s*\{[\s\S]*max-width:\s*min\(100%,\s*1480px\);/s)
  assert.match(baseStyles, /\.VPDoc\s+\.content\s*\{[\s\S]*max-width:\s*880px;/s)
  assert.match(baseStyles, /\.VPDoc:not\(\.has-sidebar\):has\(\.publication-hero\)\s+\.container\s*\{[\s\S]*max-width:\s*min\(100%,\s*1480px\);/s)
  assert.match(baseStyles, /\.VPDoc\s+\.content:has\(\.publication-hero\)\s*\{[\s\S]*max-width:\s*min\(100%,\s*1360px\);/s)
  assert.match(baseStyles, /\.VPDoc\s+\.content:has\(\.publication-hero\)\s+\.content-container\s*\{[\s\S]*max-width:\s*none;/s)
  assert.match(themeStyles, /\.VPDoc\s+\.home-whitepaper\s*\{[\s\S]*width:\s*100%;[\s\S]*max-width:\s*1360px;/s)
})

test('top nav active state is an underline rail instead of rounded pill highlight', () => {
  assert.match(themeStyles, /\.VPNavBarMenuLink\.active::after\s*\{[\s\S]*height:\s*2px;/s)
  assert.doesNotMatch(themeStyles, /\.VPNavBarMenuLink\.active\s*\{[^}]*border-radius:\s*999px;/s)
  assert.doesNotMatch(themeStyles, /\.VPNavBarMenuLink\.active\s*\{[^}]*background:\s*color-mix\([^}]*--fq-c-brand-soft/s)
})

test('theme tokens use OKLCH publication colors with explicit dual-mode diagram contrast', () => {
  for (const token of ['--fq-c-canvas', '--fq-c-surface', '--fq-c-brand', '--fq-c-info']) {
    assert.match(tokensSource, new RegExp(`${token}:\\s*oklch\\(`))
  }

  assert.match(tokensSource, /\.dark\s*\{[\s\S]*--fq-diagram-text:\s*oklch\(/s)
  assert.match(tokensSource, /--fq-diagram-glow:\s*color-mix\(/)
})
