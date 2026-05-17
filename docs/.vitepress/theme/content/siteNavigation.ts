import { localeText, themeFoundation, type LocaleKey, type SidebarTextKey } from './siteContent'

export { localeText, themeFoundation, type LocaleKey }

const sharedLinks = {
  whitepaper: {
    path: themeFoundation.sections.whitepaper.path,
    labels: { zh: localeText.zh.nav.whitepaper, en: localeText.en.nav.whitepaper },
  },
  architecture: {
    path: themeFoundation.sections.architecture.path,
    labels: { zh: localeText.zh.nav.architecture, en: localeText.en.nav.architecture },
  },
  algorithms: {
    path: themeFoundation.sections.algorithms.path,
    labels: { zh: localeText.zh.nav.algorithms, en: localeText.en.nav.algorithms },
  },
  performance: {
    path: themeFoundation.sections.performance.path,
    labels: { zh: localeText.zh.nav.performance, en: localeText.en.nav.performance },
  },
  orientationNav: {
    path: 'orientation/',
    labels: { zh: localeText.zh.nav.orientation, en: localeText.en.nav.orientation },
  },
  whitepaperNav: {
    path: 'whitepaper/',
    labels: { zh: localeText.zh.nav.whitepaper, en: localeText.en.nav.whitepaper },
  },
  referenceNav: {
    path: 'reference/',
    labels: { zh: localeText.zh.nav.reference, en: localeText.en.nav.reference },
  },
  researchNav: {
    path: 'research/',
    labels: { zh: localeText.zh.nav.research, en: localeText.en.nav.research },
  },
  why: {
    path: 'why-fastqtools/',
    labels: { zh: localeText.zh.sidebar.whyFastqtools, en: localeText.en.sidebar.whyFastqtools },
  },
  knowledgeMap: {
    path: 'knowledge-map/',
    labels: { zh: localeText.zh.sidebar.knowledgeMap, en: localeText.en.sidebar.knowledgeMap },
  },
  workflows: {
    path: 'workflows/',
    labels: { zh: localeText.zh.sidebar.workflows, en: localeText.en.sidebar.workflows },
  },
  benchmarkReport: {
    path: 'performance/benchmark-report',
    labels: { zh: localeText.zh.sidebar.benchmarkReport, en: localeText.en.sidebar.benchmarkReport },
  },
  guideOverview: {
    path: 'guide/',
    labels: { zh: localeText.zh.sidebar.guideOverview, en: localeText.en.sidebar.guideOverview },
  },
  gettingStarted: {
    path: 'guide/getting-started',
    labels: { zh: localeText.zh.sidebar.gettingStarted, en: localeText.en.sidebar.gettingStarted },
  },
  cliReference: {
    path: 'guide/cli-reference',
    labels: { zh: localeText.zh.sidebar.cliReference, en: localeText.en.sidebar.cliReference },
  },
  configuration: {
    path: 'guide/configuration',
    labels: { zh: localeText.zh.sidebar.configuration, en: localeText.en.sidebar.configuration },
  },
  deployment: {
    path: 'guide/deployment',
    labels: { zh: localeText.zh.sidebar.deployment, en: localeText.en.sidebar.deployment },
  },
  apiOverview: {
    path: 'api/overview',
    labels: { zh: localeText.zh.sidebar.apiOverview, en: localeText.en.sidebar.apiOverview },
  },
  ioModule: {
    path: 'api/io',
    labels: { zh: localeText.zh.sidebar.ioModule, en: localeText.en.sidebar.ioModule },
  },
  processing: {
    path: 'api/processing',
    labels: { zh: localeText.zh.sidebar.processing, en: localeText.en.sidebar.processing },
  },
  statistics: {
    path: 'api/statistics',
    labels: { zh: localeText.zh.sidebar.statistics, en: localeText.en.sidebar.statistics },
  },
  core: {
    path: 'api/core',
    labels: { zh: localeText.zh.sidebar.core, en: localeText.en.sidebar.core },
  },
  devOverview: {
    path: 'dev/',
    labels: { zh: localeText.zh.sidebar.devOverview, en: localeText.en.sidebar.devOverview },
  },
  developerGuide: {
    path: 'dev/',
    labels: { zh: localeText.zh.sidebar.developer, en: localeText.en.sidebar.developer },
  },
  devArchitecture: {
    path: 'dev/architecture',
    labels: { zh: localeText.zh.sidebar.devArchitecture, en: localeText.en.sidebar.devArchitecture },
  },
  design: {
    path: 'dev/design',
    labels: { zh: localeText.zh.sidebar.design, en: localeText.en.sidebar.design },
  },
  build: {
    path: 'dev/build',
    labels: { zh: localeText.zh.sidebar.build, en: localeText.en.sidebar.build },
  },
  testing: {
    path: 'dev/testing',
    labels: { zh: localeText.zh.sidebar.testing, en: localeText.en.sidebar.testing },
  },
  codingStandards: {
    path: 'dev/coding-standards',
    labels: { zh: localeText.zh.sidebar.codingStandards, en: localeText.en.sidebar.codingStandards },
  },
  localTooling: {
    path: 'dev/local-tooling',
    labels: { zh: localeText.zh.sidebar.localTooling, en: localeText.en.sidebar.localTooling },
  },
  qualityTools: {
    path: 'dev/quality-tools',
    labels: { zh: localeText.zh.sidebar.qualityTools, en: localeText.en.sidebar.qualityTools },
  },
  gitWorkflow: {
    path: 'dev/git-guidelines',
    labels: { zh: localeText.zh.sidebar.gitWorkflow, en: localeText.en.sidebar.gitWorkflow },
  },
  devcontainer: {
    path: 'dev/devcontainer',
    labels: { zh: localeText.zh.sidebar.devcontainer, en: localeText.en.sidebar.devcontainer },
  },
  benchmarkGuide: {
    path: 'dev/benchmark-guide',
    labels: { zh: localeText.zh.sidebar.benchmarkGuide, en: localeText.en.sidebar.benchmarkGuide },
  },
  resourcesOverview: {
    path: 'resources/',
    labels: { zh: localeText.zh.sidebar.resourcesOverview, en: localeText.en.sidebar.resourcesOverview },
  },
  contributing: {
    path: 'contributing/',
    labels: { zh: localeText.zh.sidebar.contributing, en: localeText.en.sidebar.contributing },
  },
  changelog: {
    path: 'release-notes/changelog',
    labels: { zh: localeText.zh.sidebar.changelog, en: localeText.en.sidebar.changelog },
  },
  agentsDomain: {
    path: 'agents/domain',
    labels: { zh: localeText.zh.sidebar.domain, en: localeText.en.sidebar.domain },
  },
  issueTracker: {
    path: 'agents/issue-tracker',
    labels: { zh: localeText.zh.sidebar.issueTracker, en: localeText.en.sidebar.issueTracker },
  },
  triageLabels: {
    path: 'agents/triage-labels',
    labels: { zh: localeText.zh.sidebar.triageLabels, en: localeText.en.sidebar.triageLabels },
  },
  archiveOverview: {
    path: 'archive/',
    labels: { zh: localeText.zh.sidebar.archiveOverview, en: localeText.en.sidebar.archiveOverview },
  },
  migrationNotice: {
    path: 'archive/migration-notice-2026-04-17',
    labels: { zh: localeText.zh.sidebar.migrationNotice, en: localeText.en.sidebar.migrationNotice },
  },
} as const

export type SharedLinkId = keyof typeof sharedLinks

export const topNavLinkIds = [
  'whitepaper',
  'architecture',
  'algorithms',
  'performance',
  'referenceNav',
  'researchNav',
] as const satisfies readonly SharedLinkId[]

export const orientationSidebarGroups = [
  {
    headingKey: 'orientationHub',
    items: ['orientationNav', 'whitepaper', 'architecture', 'algorithms', 'performance', 'referenceNav', 'researchNav'],
  },
] as const satisfies readonly { headingKey: SidebarTextKey; items: readonly SharedLinkId[] }[]

export const whitepaperSidebarGroups = [
  { headingKey: 'whitepaperHub', items: ['whitepaper', 'orientationNav', 'whitepaperNav', 'why'] },
  { headingKey: 'narrativeContext', items: ['architecture', 'algorithms', 'performance', 'referenceNav', 'researchNav'] },
] as const satisfies readonly { headingKey: SidebarTextKey; items: readonly SharedLinkId[] }[]

export const architectureSidebarGroups = [
  { headingKey: 'architectureHub', items: ['architecture', 'knowledgeMap'] },
  { headingKey: 'systemDesign', items: ['whitepaper', 'performance', 'referenceNav'] },
] as const satisfies readonly { headingKey: SidebarTextKey; items: readonly SharedLinkId[] }[]

export const algorithmsSidebarGroups = [
  { headingKey: 'algorithmsHub', items: ['algorithms', 'workflows'] },
  { headingKey: 'operatorRoutes', items: ['gettingStarted', 'cliReference', 'performance', 'referenceNav'] },
] as const satisfies readonly { headingKey: SidebarTextKey; items: readonly SharedLinkId[] }[]

export const performanceSidebarGroups = [
  { headingKey: 'performanceHub', items: ['performance', 'benchmarkReport'] },
  { headingKey: 'evidenceTrail', items: ['whitepaper', 'architecture', 'researchNav'] },
] as const satisfies readonly { headingKey: SidebarTextKey; items: readonly SharedLinkId[] }[]

export const referenceSidebarGroups = [
  { headingKey: 'referenceHub', items: ['referenceNav'] },
  { headingKey: 'guide', items: ['guideOverview', 'gettingStarted', 'cliReference', 'configuration', 'deployment'] },
  { headingKey: 'api', items: ['apiOverview', 'ioModule', 'processing', 'statistics', 'core'] },
  {
    headingKey: 'developer',
    items: ['devOverview', 'devArchitecture', 'design', 'build', 'testing', 'codingStandards', 'localTooling', 'qualityTools', 'gitWorkflow', 'devcontainer', 'benchmarkGuide'],
  },
  { headingKey: 'releaseNotes', items: ['changelog'] },
] as const satisfies readonly { headingKey: SidebarTextKey; items: readonly SharedLinkId[] }[]

export const researchSidebarGroups = [
  { headingKey: 'researchHub', items: ['researchNav', 'performance', 'benchmarkReport', 'resourcesOverview', 'contributing'] },
  { headingKey: 'agents', items: ['agentsDomain', 'issueTracker', 'triageLabels'] },
  { headingKey: 'archive', items: ['archiveOverview', 'migrationNotice'] },
] as const satisfies readonly { headingKey: SidebarTextKey; items: readonly SharedLinkId[] }[]

export const homepagePillarLinkIds = {
  performance: 'performance',
  workflows: 'workflows',
  architecture: 'architecture',
  reference: 'referenceNav',
} as const satisfies Record<string, SharedLinkId>

export const knowledgeMapSections = [
  { id: 'evaluate', href: 'whitepaper', items: ['whitepaper', 'why', 'architecture', 'performance'] },
  { id: 'operate', href: 'algorithms', items: ['algorithms', 'workflows', 'gettingStarted', 'cliReference'] },
  { id: 'reference', href: 'referenceNav', items: ['referenceNav', 'apiOverview', 'developerGuide'] },
  { id: 'extend', href: 'resourcesOverview', items: ['resourcesOverview', 'contributing', 'agentsDomain'] },
] as const satisfies readonly { id: string; href: SharedLinkId; items: readonly SharedLinkId[] }[]

export const workflowPathLinks = {
  evaluate: { primary: 'gettingStarted', secondary: 'why' },
  operate: { primary: 'workflows', secondary: 'cliReference' },
  contribute: { primary: 'build', secondary: 'testing' },
} as const satisfies Record<string, { primary: SharedLinkId; secondary: SharedLinkId }>

export type ResourceHubItem =
  | { type: 'doc'; id: SharedLinkId }
  | { type: 'external'; href: string; labels: Record<LocaleKey, string> }

export const resourceHubGroups = [
  {
    id: 'reference',
    href: 'referenceNav',
    items: [{ type: 'doc', id: 'gettingStarted' }, { type: 'doc', id: 'apiOverview' }, { type: 'doc', id: 'developerGuide' }],
  },
  {
    id: 'collaboration',
    href: 'resourcesOverview',
    items: [
      { type: 'doc', id: 'contributing' },
      { type: 'doc', id: 'changelog' },
      {
        type: 'external',
        href: 'https://github.com/LessUp/fastq-tools',
        labels: { zh: 'GitHub 仓库', en: 'GitHub Repository' },
      },
    ],
  },
  {
    id: 'automation',
    href: 'agentsDomain',
    items: [{ type: 'doc', id: 'agentsDomain' }, { type: 'doc', id: 'issueTracker' }, { type: 'doc', id: 'triageLabels' }],
  },
  {
    id: 'history',
    href: 'archiveOverview',
    items: [{ type: 'doc', id: 'archiveOverview' }, { type: 'doc', id: 'migrationNotice' }, { type: 'doc', id: 'knowledgeMap' }],
  },
] as const satisfies readonly { id: string; href: SharedLinkId; items: readonly ResourceHubItem[] }[]

const createLocalePath = (locale: LocaleKey, path: string) => {
  const trimmedPath = path.replace(/^\/+/, '')
  return `/${locale}/${trimmedPath}`
}

export const getSharedLinkHref = (locale: LocaleKey, id: SharedLinkId) => createLocalePath(locale, sharedLinks[id].path)

export const getSharedLinkLabel = (locale: LocaleKey, id: SharedLinkId) => sharedLinks[id].labels[locale]

export const getSharedLink = (locale: LocaleKey, id: SharedLinkId) => ({
  text: getSharedLinkLabel(locale, id),
  link: getSharedLinkHref(locale, id),
})
