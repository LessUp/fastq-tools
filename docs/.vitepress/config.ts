import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'
import {
  academySidebarGroups,
  getSharedLink,
  localeText,
  orientationSidebarGroups,
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
    academy: '/zh/academy/',
    reference: '/zh/reference/',
    research: '/zh/research/',
  },
  en: {
    orientation: '/en/orientation/',
    whitepaper: '/en/whitepaper/',
    academy: '/en/academy/',
    reference: '/en/reference/',
    research: '/en/research/',
  },
} as const satisfies Record<LocaleKey, Record<'orientation' | 'whitepaper' | 'academy' | 'reference' | 'research', string>>

const createOrientationSidebar = (locale: LocaleKey) => {
  return orientationSidebarGroups.map((group) => ({
    text: localeText[locale].sidebar[group.headingKey],
    items: group.items.map((id) => getSharedLink(locale, id)),
  }))
}

const createWhitepaperSidebar = (locale: LocaleKey) => {
  return whitepaperSidebarGroups.map((group) => ({
    text: localeText[locale].sidebar[group.headingKey],
    items: group.items.map((id) => getSharedLink(locale, id)),
  }))
}

const createAcademySidebar = (locale: LocaleKey) => {
  return academySidebarGroups.map((group) => ({
    text: localeText[locale].sidebar[group.headingKey],
    items: group.items.map((id) => getSharedLink(locale, id)),
  }))
}

const createReferenceSidebar = (locale: LocaleKey) => {
  return referenceSidebarGroups.map((group) => ({
    text: localeText[locale].sidebar[group.headingKey],
    items: group.items.map((id) => getSharedLink(locale, id)),
  }))
}

const createResearchSidebar = (locale: LocaleKey) => {
  return researchSidebarGroups.map((group) => ({
    text: localeText[locale].sidebar[group.headingKey],
    items: group.items.map((id) => getSharedLink(locale, id)),
  }))
}

const createTopNavActiveMatches = (locale: LocaleKey) => ({
  orientationNav: `^/${locale}/(orientation|knowledge-map)/`,
  whitepaperNav: `^/${locale}/(whitepaper|why-fastqtools|architecture)/`,
  academyNav: `^/${locale}/(academy|workflows)/`,
  referenceNav: `^/${locale}/(reference|guide|api|dev|release-notes)/`,
  researchNav: `^/${locale}/(research|performance|resources|agents|archive|contributing)`,
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
    [localeSidebarRoutes[locale].orientation]: createOrientationSidebar(locale),
    [localeSidebarRoutes[locale].whitepaper]: createWhitepaperSidebar(locale),
    [localeSidebarRoutes[locale].academy]: createAcademySidebar(locale),
    [localeSidebarRoutes[locale].reference]: createReferenceSidebar(locale),
    [localeSidebarRoutes[locale].research]: createResearchSidebar(locale),
    [`/${locale}/why-fastqtools/`]: createWhitepaperSidebar(locale),
    [`/${locale}/architecture/`]: createWhitepaperSidebar(locale),
    [`/${locale}/workflows/`]: createAcademySidebar(locale),
    [`/${locale}/knowledge-map/`]: createOrientationSidebar(locale),
    [`/${locale}/performance/`]: createResearchSidebar(locale),
    [`/${locale}/guide/`]: createReferenceSidebar(locale),
    [`/${locale}/api/`]: createReferenceSidebar(locale),
    [`/${locale}/dev/`]: createReferenceSidebar(locale),
    [`/${locale}/release-notes/`]: createReferenceSidebar(locale),
    [`/${locale}/resources/`]: createResearchSidebar(locale),
    [`/${locale}/contributing`]: createResearchSidebar(locale),
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
    plugins: [llmstxt()],
    build: {
      // Increase chunk size warning limit for mermaid plugin
      chunkSizeWarningLimit: 700,
    },
  },
}))
