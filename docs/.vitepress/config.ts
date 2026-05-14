import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'
import {
  getSharedLink,
  localeText,
  narrativeLinkIds,
  performanceLinkIds,
  topNavLinkIds,
  referenceSidebarGroups,
  resourceSidebarGroups,
  type LocaleKey,
} from './theme/content/siteNavigation'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

const createNarrativeItems = (locale: LocaleKey) => {
  return narrativeLinkIds.map((id) => getSharedLink(locale, id))
}

const createNarrativeSidebar = (locale: LocaleKey) => {
  const text = localeText[locale].sidebar

  return [
    {
      text: text.storyHub,
      items: createNarrativeItems(locale),
    },
  ]
}

const createPerformanceSidebar = (locale: LocaleKey) => {
  const text = localeText[locale].sidebar

  return [
    ...createNarrativeSidebar(locale),
    {
      text: text.evidenceHub,
      items: performanceLinkIds.map((id) => getSharedLink(locale, id)),
    },
  ]
}

const createReferenceSidebar = (locale: LocaleKey) => {
  return referenceSidebarGroups.map((group) => ({
    text: localeText[locale].sidebar[group.headingKey],
    items: group.items.map((id) => getSharedLink(locale, id)),
  }))
}

const createResourcesSidebar = (locale: LocaleKey) => {
  return [
    ...createNarrativeSidebar(locale),
    ...resourceSidebarGroups.map((group) => ({
      text: localeText[locale].sidebar[group.headingKey],
      items: group.items.map((id) => getSharedLink(locale, id)),
    })),
  ]
}

const createTopNavActiveMatches = (locale: LocaleKey) => ({
  why: `^/${locale}/why-fastqtools/`,
  architecture: `^/${locale}/architecture/`,
  workflows: `^/${locale}/workflows/`,
  knowledgeMap: `^/${locale}/knowledge-map/`,
  performance: `^/${locale}/performance/`,
  referenceNav: `^/${locale}/(reference|guide|api|dev|release-notes)/`,
  resourcesNav: `^/${locale}/(resources|agents|archive|contributing)`,
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
    [`/${locale}/why-fastqtools/`]: createNarrativeSidebar(locale),
    [`/${locale}/architecture/`]: createNarrativeSidebar(locale),
    [`/${locale}/workflows/`]: createNarrativeSidebar(locale),
    [`/${locale}/knowledge-map/`]: createNarrativeSidebar(locale),
    [`/${locale}/performance/`]: createPerformanceSidebar(locale),
    [`/${locale}/reference/`]: createReferenceSidebar(locale),
    [`/${locale}/guide/`]: createReferenceSidebar(locale),
    [`/${locale}/api/`]: createReferenceSidebar(locale),
    [`/${locale}/dev/`]: createReferenceSidebar(locale),
    [`/${locale}/release-notes/`]: createReferenceSidebar(locale),
    [`/${locale}/resources/`]: createResourcesSidebar(locale),
    [`/${locale}/contributing`]: createResourcesSidebar(locale),
    [`/${locale}/agents/`]: createResourcesSidebar(locale),
    [`/${locale}/archive/`]: createResourcesSidebar(locale),
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
