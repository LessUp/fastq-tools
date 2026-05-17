import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'
import {
  algorithmsSidebarGroups,
  architectureSidebarGroups,
  getSharedLink,
  localeText,
  orientationSidebarGroups,
  performanceSidebarGroups,
  researchSidebarGroups,
  referenceSidebarGroups,
  topNavLinkIds,
  whitepaperSidebarGroups,
  type LocaleKey,
} from './theme/content/siteNavigation'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

const localeSidebarRoutes = {
  zh: {
    orientation: '/zh/orientation/',
    whitepaper: '/zh/whitepaper/',
    architecture: '/zh/architecture/',
    algorithms: '/zh/algorithms/',
    performance: '/zh/performance/',
    reference: '/zh/reference/',
    research: '/zh/research/',
  },
  en: {
    orientation: '/en/orientation/',
    whitepaper: '/en/whitepaper/',
    architecture: '/en/architecture/',
    algorithms: '/en/algorithms/',
    performance: '/en/performance/',
    reference: '/en/reference/',
    research: '/en/research/',
  },
} as const satisfies Record<LocaleKey, Record<'orientation' | 'whitepaper' | 'architecture' | 'algorithms' | 'performance' | 'reference' | 'research', string>>

const createSidebar = (
  locale: LocaleKey,
  groups: readonly { headingKey: keyof typeof localeText.en.sidebar; items: readonly Parameters<typeof getSharedLink>[1][] }[],
) => groups.map((group) => ({
  text: localeText[locale].sidebar[group.headingKey],
  items: group.items.map((id) => getSharedLink(locale, id)),
}))

const createOrientationSidebar = (locale: LocaleKey) => createSidebar(locale, orientationSidebarGroups)
const createWhitepaperSidebar = (locale: LocaleKey) => createSidebar(locale, whitepaperSidebarGroups)
const createArchitectureSidebar = (locale: LocaleKey) => createSidebar(locale, architectureSidebarGroups)
const createAlgorithmsSidebar = (locale: LocaleKey) => createSidebar(locale, algorithmsSidebarGroups)
const createPerformanceSidebar = (locale: LocaleKey) => createSidebar(locale, performanceSidebarGroups)
const createReferenceSidebar = (locale: LocaleKey) => createSidebar(locale, referenceSidebarGroups)
const createResearchSidebar = (locale: LocaleKey) => createSidebar(locale, researchSidebarGroups)

const createTopNavActiveMatches = (locale: LocaleKey) => ({
  whitepaper: `^/${locale}/($|(whitepaper|orientation|why-fastqtools)(/|$))`,
  architecture: `^/${locale}/(architecture|knowledge-map)(/|$)`,
  algorithms: `^/${locale}/(algorithms|academy|workflows)(/|$)`,
  performance: `^/${locale}/performance(/|$)`,
  referenceNav: `^/${locale}/(reference|guide|api|dev|release-notes)/`,
  researchNav: `^/${locale}/(research|resources|agents|archive|contributing)(/|$)`,
} as const satisfies Record<(typeof topNavLinkIds)[number], string>)

const createLocaleThemeConfig = (locale: LocaleKey) => ({
  nav: topNavLinkIds.map((id) => {
    const link = getSharedLink(locale, id)

    return {
      ...link,
      activeMatch: createTopNavActiveMatches(locale)[id],
    }
  }),
  sidebar: {
    [localeSidebarRoutes[locale].whitepaper]: createWhitepaperSidebar(locale),
    [localeSidebarRoutes[locale].architecture]: createArchitectureSidebar(locale),
    [localeSidebarRoutes[locale].algorithms]: createAlgorithmsSidebar(locale),
    [localeSidebarRoutes[locale].performance]: createPerformanceSidebar(locale),
    [localeSidebarRoutes[locale].reference]: createReferenceSidebar(locale),
    [localeSidebarRoutes[locale].research]: createResearchSidebar(locale),
    [localeSidebarRoutes[locale].orientation]: createOrientationSidebar(locale),
    [`/${locale}/academy/`]: createAlgorithmsSidebar(locale),
    [`/${locale}/why-fastqtools/`]: createWhitepaperSidebar(locale),
    [`/${locale}/architecture/`]: createArchitectureSidebar(locale),
    [`/${locale}/workflows/`]: createAlgorithmsSidebar(locale),
    [`/${locale}/knowledge-map/`]: createArchitectureSidebar(locale),
    [`/${locale}/performance/`]: createPerformanceSidebar(locale),
    [`/${locale}/guide/`]: createReferenceSidebar(locale),
    [`/${locale}/api/`]: createReferenceSidebar(locale),
    [`/${locale}/dev/`]: createReferenceSidebar(locale),
    [`/${locale}/release-notes/`]: createReferenceSidebar(locale),
    [`/${locale}/resources/`]: createResearchSidebar(locale),
    [`/${locale}/contributing/`]: createResearchSidebar(locale),
    [`/${locale}/agents/`]: createResearchSidebar(locale),
    [`/${locale}/archive/`]: createResearchSidebar(locale),
  },
  editLink: {
    pattern: 'https://github.com/LessUp/fastq-tools/edit/master/docs/:path',
    text: localeText[locale].editLinkText,
  },
  footer: {
    message: 'MIT License © LessUp',
    copyright: localeText[locale].footerCopyright,
  },
})

export default withMermaid(defineConfig({
  base,
  title: 'FastQTools',
  description: 'High-performance FASTQ processing toolkit for sequencing QC',
  srcExclude: ['superpowers/**', 'assets/**/README.md'],

  locales: {
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: 'FastQTools',
      description: '高性能 FASTQ 处理工具，面向生物信息学 QC 工作流',
      themeConfig: createLocaleThemeConfig('zh'),
    },
    en: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      title: 'FastQTools',
      description: 'High-performance FASTQ processing toolkit for sequencing QC',
      themeConfig: createLocaleThemeConfig('en'),
    },
  },

  themeConfig: {
    outline: [2, 3],
    search: { provider: 'local' },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/fastq-tools' },
    ],
  },

  vite: {
    plugins: [llmstxt({
      ignoreFiles: ['superpowers/**', 'assets/**/README.md'],
    })],
    build: {
      // Increase chunk size warning limit for mermaid plugin
      chunkSizeWarningLimit: 700,
    },
  },
}))
