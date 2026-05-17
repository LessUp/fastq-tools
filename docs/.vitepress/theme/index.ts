import DefaultTheme from 'vitepress/theme'
import { inBrowser } from 'vitepress'
import type { EnhanceAppContext, Theme } from 'vitepress'
import SiteHeroPanel from './components/SiteHeroPanel.vue'
import EvidenceStrip from './components/EvidenceStrip.vue'
import PillarGrid from './components/PillarGrid.vue'
import KnowledgeMap from './components/KnowledgeMap.vue'
import WorkflowPaths from './components/WorkflowPaths.vue'
import ResourceHub from './components/ResourceHub.vue'
import CitationStrip from './components/CitationStrip.vue'
import DiagramFrame from './components/DiagramFrame.vue'
import SectionLandingGrid from './components/SectionLandingGrid.vue'
import WhitepaperHero from './components/WhitepaperHero.vue'
import SystemSnapshot from './components/SystemSnapshot.vue'
import AlgorithmPillars from './components/AlgorithmPillars.vue'
import EvidenceOverview from './components/EvidenceOverview.vue'
import ReadingTracks from './components/ReadingTracks.vue'
import ResearchRail from './components/ResearchRail.vue'
import SystemArchitectureDiagram from './components/SystemArchitectureDiagram.vue'
import ReferenceList from './components/ReferenceList.vue'
import ReferenceBadge from './components/ReferenceBadge.vue'
import { referenceCatalog } from './content/references'
import { themeFoundation } from './content/siteContent'
import './style.css'

const LANG_KEY = 'vitepress:lang-pref'
const THEME_FOUNDATION_KEY = 'fq-theme-foundation'
const REFERENCE_CATALOG_KEY = 'fq-reference-catalog'

type SupportedLocale = 'zh' | 'en'

function detectBrowserLang(): SupportedLocale {
  const navLang = inBrowser
    ? (navigator.language || (navigator as Navigator & { userLanguage?: string }).userLanguage || '').toLowerCase()
    : ''
  return navLang.startsWith('zh') ? 'zh' : 'en'
}

function getLangPreference(): SupportedLocale | null {
  if (!inBrowser) return null
  try {
    const stored = localStorage.getItem(LANG_KEY)
    return stored === 'zh' || stored === 'en' ? stored : null
  } catch {
    return null
  }
}

function setLangPreference(lang: SupportedLocale): void {
  if (!inBrowser) return
  try {
    localStorage.setItem(LANG_KEY, lang)
  } catch {
    // localStorage may be unavailable.
  }
}

function normalizeBase(base: string): string {
  if (base === '/') return ''
  return base.replace(/\/$/, '')
}

function resolveLocaleFromPath(path: string, base: string): SupportedLocale | null {
  const normalizedBase = normalizeBase(base)
  const relativePath = normalizedBase && path.startsWith(normalizedBase)
    ? path.slice(normalizedBase.length) || '/'
    : path

  if (relativePath === '/zh' || relativePath === '/zh/' || relativePath.startsWith('/zh/')) return 'zh'
  if (relativePath === '/en' || relativePath === '/en/' || relativePath.startsWith('/en/')) return 'en'
  return null
}

function isRootPath(path: string, base: string): boolean {
  const normalizedBase = normalizeBase(base)

  return path === '/' ||
    path === '/index.html' ||
    path === normalizedBase ||
    path === `${normalizedBase}/` ||
    path === `${normalizedBase}/index.html`
}

function installLanguagePreference({ router, siteData }: EnhanceAppContext): void {
  if (!inBrowser) return

  const base = siteData.value.base || '/'
  const updatePreferenceFromPath = (path: string) => {
    const locale = resolveLocaleFromPath(path, base)
    if (locale) setLangPreference(locale)
  }

  updatePreferenceFromPath(window.location.pathname)

  if (isRootPath(window.location.pathname, base)) {
    const targetLocale = getLangPreference() || detectBrowserLang()
    const target = `${normalizeBase(base)}/${targetLocale}/`
    window.location.replace(target)
    return
  }

  const previousAfterRouteChanged = router.onAfterRouteChanged
  router.onAfterRouteChanged = (to: string) => {
    previousAfterRouteChanged?.(to)
    if (isRootPath(to, base)) {
      const targetLocale = getLangPreference() || detectBrowserLang()
      const target = `${normalizeBase(base)}/${targetLocale}/`
      window.location.replace(target)
      return
    }
    updatePreferenceFromPath(to)
  }
}

const theme: Theme = {
  extends: DefaultTheme,
  enhanceApp(ctx) {
    DefaultTheme.enhanceApp?.(ctx)

    ctx.app.component('SiteHeroPanel', SiteHeroPanel)
    ctx.app.component('EvidenceStrip', EvidenceStrip)
    ctx.app.component('PillarGrid', PillarGrid)
    ctx.app.component('KnowledgeMap', KnowledgeMap)
    ctx.app.component('WorkflowPaths', WorkflowPaths)
    ctx.app.component('ResourceHub', ResourceHub)
    ctx.app.component('CitationStrip', CitationStrip)
    ctx.app.component('DiagramFrame', DiagramFrame)
    ctx.app.component('SectionLandingGrid', SectionLandingGrid)
    ctx.app.component('WhitepaperHero', WhitepaperHero)
    ctx.app.component('SystemSnapshot', SystemSnapshot)
    ctx.app.component('AlgorithmPillars', AlgorithmPillars)
    ctx.app.component('EvidenceOverview', EvidenceOverview)
    ctx.app.component('ReadingTracks', ReadingTracks)
    ctx.app.component('ResearchRail', ResearchRail)
    ctx.app.component('SystemArchitectureDiagram', SystemArchitectureDiagram)
    ctx.app.component('ReferenceList', ReferenceList)
    ctx.app.component('ReferenceBadge', ReferenceBadge)
    ctx.app.provide(THEME_FOUNDATION_KEY, themeFoundation)
    ctx.app.provide(REFERENCE_CATALOG_KEY, referenceCatalog)

    installLanguagePreference(ctx)
  },
}

export default theme
