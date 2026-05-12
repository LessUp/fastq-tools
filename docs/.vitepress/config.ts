import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

export default withMermaid(defineConfig({
  base,
  title: 'FastQTools',
  description: 'High-performance FASTQ processing toolkit for sequencing QC',

  locales: {
    root: {
      label: '简体中文',
      lang: 'zh-CN',
      title: 'FastQTools',
      description: '高性能 FASTQ 处理工具，面向生物信息学 QC 工作流',
      themeConfig: {
        nav: [
          { text: '指南', link: '/guide/getting-started', activeMatch: '/guide/' },
          { text: 'API', link: '/api/overview', activeMatch: '/api/' },
          { text: '开发', link: '/dev/', activeMatch: '/dev/' },
          { text: '性能', link: '/performance/benchmark-report', activeMatch: '/performance/' },
          { text: '发布说明', link: '/release-notes/changelog', activeMatch: '/release-notes/' },
          { text: 'Agent', link: '/agents/domain', activeMatch: '/agents/' },
        ],
        sidebar: {
          '/guide/': [
            {
              text: '指南',
              items: [
                { text: '快速开始', link: '/guide/getting-started' },
                { text: 'CLI 参考', link: '/guide/cli-reference' },
                { text: '配置', link: '/guide/configuration' },
                { text: '部署', link: '/guide/deployment' },
              ],
            },
          ],
          '/api/': [
            {
              text: 'API 参考',
              items: [
                { text: '概览', link: '/api/overview' },
                { text: 'IO 模块', link: '/api/io' },
                { text: '处理模块', link: '/api/processing' },
                { text: '统计模块', link: '/api/statistics' },
                { text: '核心模块', link: '/api/core' },
              ],
            },
          ],
          '/dev/': [
            {
              text: '开发者指南',
              items: [
                { text: '概览', link: '/dev/' },
                { text: '架构设计', link: '/dev/architecture' },
                { text: '核心设计', link: '/dev/design' },
                { text: '构建指南', link: '/dev/build' },
                { text: '测试策略', link: '/dev/testing' },
                { text: '编码规范', link: '/dev/coding-standards' },
                { text: '本地工具', link: '/dev/local-tooling' },
                { text: '代码质量', link: '/dev/quality-tools' },
                { text: 'Git 工作流', link: '/dev/git-guidelines' },
                { text: 'DevContainer', link: '/dev/devcontainer' },
                { text: 'Benchmark', link: '/dev/benchmark-guide' },
              ],
            },
          ],
          '/performance/': [
            {
              text: '性能',
              items: [
                { text: 'Benchmark 报告', link: '/performance/benchmark-report' },
              ],
            },
          ],
          '/agents/': [
            {
              text: 'Agent',
              items: [
                { text: '领域模型', link: '/agents/domain' },
                { text: 'Issue Tracker', link: '/agents/issue-tracker' },
                { text: '分类标签', link: '/agents/triage-labels' },
              ],
            },
          ],
          '/release-notes/': [
            {
              text: '发布说明',
              items: [
                { text: '变更记录', link: '/release-notes/changelog' },
              ],
            },
          ],
          '/archive/': [
            {
              text: '归档',
              items: [
                { text: '概览', link: '/archive/' },
                { text: '迁移通知', link: '/archive/migration-notice-2026-04-17' },
              ],
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/LessUp/fastq-tools/edit/master/docs/:path',
          text: '在 GitHub 上编辑此页',
        },
        footer: {
          message: 'MIT License © LessUp',
          copyright: '基于 VitePress 构建',
        },
      },
    },
    en: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      title: 'FastQTools',
      description: 'High-performance FASTQ processing toolkit for sequencing QC',
      themeConfig: {
        nav: [
          { text: 'Guide', link: '/en/guide/getting-started', activeMatch: '/en/guide/' },
          { text: 'API', link: '/en/api/overview', activeMatch: '/en/api/' },
          { text: 'Dev', link: '/en/dev/', activeMatch: '/en/dev/' },
          { text: 'Performance', link: '/en/performance/benchmark-report', activeMatch: '/en/performance/' },
          { text: 'Release Notes', link: '/en/release-notes/changelog', activeMatch: '/en/release-notes/' },
          { text: 'Agent', link: '/en/agents/domain', activeMatch: '/en/agents/' },
        ],
        sidebar: {
          '/en/guide/': [
            {
              text: 'Guide',
              items: [
                { text: 'Getting Started', link: '/en/guide/getting-started' },
                { text: 'CLI Reference', link: '/en/guide/cli-reference' },
                { text: 'Configuration', link: '/en/guide/configuration' },
                { text: 'Deployment', link: '/en/guide/deployment' },
              ],
            },
          ],
          '/en/api/': [
            {
              text: 'API Reference',
              items: [
                { text: 'Overview', link: '/en/api/overview' },
                { text: 'IO Module', link: '/en/api/io' },
                { text: 'Processing', link: '/en/api/processing' },
                { text: 'Statistics', link: '/en/api/statistics' },
                { text: 'Core', link: '/en/api/core' },
              ],
            },
          ],
          '/en/dev/': [
            {
              text: 'Developer Guide',
              items: [
                { text: 'Overview', link: '/en/dev/' },
                { text: 'Architecture', link: '/en/dev/architecture' },
                { text: 'Design', link: '/en/dev/design' },
                { text: 'Build', link: '/en/dev/build' },
                { text: 'Testing', link: '/en/dev/testing' },
                { text: 'Coding Standards', link: '/en/dev/coding-standards' },
                { text: 'Local Tooling', link: '/en/dev/local-tooling' },
                { text: 'Quality Tools', link: '/en/dev/quality-tools' },
                { text: 'Git Workflow', link: '/en/dev/git-guidelines' },
                { text: 'DevContainer', link: '/en/dev/devcontainer' },
                { text: 'Benchmark', link: '/en/dev/benchmark-guide' },
              ],
            },
          ],
          '/en/performance/': [
            {
              text: 'Performance',
              items: [
                { text: 'Benchmark Report', link: '/en/performance/benchmark-report' },
              ],
            },
          ],
          '/en/agents/': [
            {
              text: 'Agent',
              items: [
                { text: 'Domain', link: '/en/agents/domain' },
                { text: 'Issue Tracker', link: '/en/agents/issue-tracker' },
                { text: 'Triage Labels', link: '/en/agents/triage-labels' },
              ],
            },
          ],
          '/en/release-notes/': [
            {
              text: 'Release Notes',
              items: [
                { text: 'Changelog', link: '/en/release-notes/changelog' },
              ],
            },
          ],
          '/en/archive/': [
            {
              text: 'Archive',
              items: [
                { text: 'Overview', link: '/en/archive/' },
                { text: 'Migration Notice', link: '/en/archive/migration-notice-2026-04-17' },
              ],
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/LessUp/fastq-tools/edit/master/docs/:path',
          text: 'Edit this page on GitHub',
        },
        footer: {
          message: 'MIT License © LessUp',
          copyright: 'Built with VitePress',
        },
      },
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
  },

  markdown: {
    config: (md) => {
      // mermaid is handled by vitepress-plugin-mermaid
    },
  },
}))
