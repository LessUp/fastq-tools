<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { BookOpen, Cpu, GitBranch, ShieldCheck } from 'lucide-vue-next'
import { getSharedLinkHref, homepagePillarLinkIds, type LocaleKey } from '../content/siteNavigation'

type Pillar = {
  title: string
  summary: string
  bullets: string[]
  linkId: keyof typeof homepagePillarLinkIds
  linkLabel: string
  icon: typeof Cpu
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    title: 'Homepage decision pillars',
    pillars: [
      {
        title: 'Performance you can size',
        summary: 'The docs lead with bounded, reproducible benchmark signals instead of broad marketing claims.',
        bullets: ['Read, write, filter, and stat paths all have maintained headline figures.', 'Performance pages explain methodology, environment, and reproducibility boundaries.'],
        linkId: 'performance',
        linkLabel: 'See performance evidence',
        icon: Cpu,
      },
      {
        title: 'Workflow-first adoption',
        summary: 'Readers can move from the first stat run to ongoing filter operations without digging through unrelated material.',
        bullets: ['Narrative pages stay separate from deep execution reference.', 'Homepage cards point directly to getting-started and workflow paths.'],
        linkId: 'workflows',
        linkLabel: 'Explore workflows',
        icon: BookOpen,
      },
      {
        title: 'Specification-backed maintenance',
        summary: 'Current product behavior maps back to maintained baseline specifications for product, architecture, API, and testing.',
        bullets: ['Baseline specs remain the maintained source of truth.', 'Archive pages stay discoverable without being presented as current guidance.'],
        linkId: 'architecture',
        linkLabel: 'Review architecture',
        icon: ShieldCheck,
      },
      {
        title: 'Practical developer entry points',
        summary: 'Teams evaluating integration can jump straight to CLI, API, build, and test entry points.',
        bullets: ['C++ API and CLI references are grouped under the reference hub.', 'scripts/core remains the supported entry path for build, test, and lint.'],
        linkId: 'reference',
        linkLabel: 'Open reference hub',
        icon: GitBranch,
      },
    ] satisfies Pillar[],
  },
  zh: {
    title: '首页决策支柱',
    pillars: [
      {
        title: '可量化的性能证据',
        summary: '文档优先给出可复核、可限定边界的 benchmark 信号，而不是泛化的营销描述。',
        bullets: ['读取、写出、过滤与统计路径都有维护中的代表性数字。', '性能页面会解释方法学、环境与复现边界。'],
        linkId: 'performance',
        linkLabel: '查看性能证据',
        icon: Cpu,
      },
      {
        title: '按工作流组织采用路径',
        summary: '读者可以从第一次 stat 运行一路走到常规 filter 流程，而不必穿过无关内容。',
        bullets: ['叙事页面与深度执行参考分层组织。', '首页卡片直接连接快速开始与工作流路径。'],
        linkId: 'workflows',
        linkLabel: '浏览工作流',
        icon: BookOpen,
      },
      {
        title: '以规范为依据的维护方式',
        summary: '当前产品行为可以回链到持续维护的产品、架构、API 与测试 baseline。',
        bullets: ['baseline 规范继续作为维护中的事实来源。', '归档内容可追溯，但不会被展示成当前指南。'],
        linkId: 'architecture',
        linkLabel: '查看架构',
        icon: ShieldCheck,
      },
      {
        title: '直接可用的开发入口',
        summary: '评估集成的团队可以快速跳到 CLI、API、构建与测试入口。',
        bullets: ['C++ API 与 CLI 参考统一收敛到参考导航。', 'build / test / lint 仍然以 scripts/core 为支持入口。'],
        linkId: 'reference',
        linkLabel: '打开参考导航',
        icon: GitBranch,
      },
    ] satisfies Pillar[],
  },
} as const

const content = computed(() => copy[props.locale])
</script>

<template>
  <section class="pillar-grid">
    <h2>{{ content.title }}</h2>
    <div class="grid">
      <article v-for="pillar in content.pillars" :key="pillar.title" class="pillar-card">
        <component :is="pillar.icon" :size="18" class="pillar-icon" />
        <h3>{{ pillar.title }}</h3>
        <p>{{ pillar.summary }}</p>
        <ul>
          <li v-for="bullet in pillar.bullets" :key="bullet">{{ bullet }}</li>
        </ul>
        <a :href="withBase(getSharedLinkHref(props.locale, homepagePillarLinkIds[pillar.linkId]))">{{ pillar.linkLabel }}</a>
      </article>
    </div>
  </section>
</template>
