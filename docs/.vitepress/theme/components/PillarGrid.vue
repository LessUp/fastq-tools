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
    title: 'Reading path pillars',
    pillars: [
      {
        title: 'Evidence before advocacy',
        summary: 'The reading path keeps bounded, reproducible benchmark signals close to the narrative instead of hiding them behind marketing-style claims.',
        bullets: ['Read, write, filter, and stat paths all keep maintained headline figures.', 'Research pages explain methodology, environment, and reproducibility boundaries.'],
        linkId: 'performance',
        linkLabel: 'See performance evidence',
        icon: Cpu,
      },
      {
        title: 'Orientation before execution',
        summary: 'Readers can choose the right section root first, then move from onboarding to routine filter operations without digging through unrelated material.',
        bullets: ['Orientation separates the narrative, learning, reference, and research layers.', 'Academy cards point directly to getting-started and workflow paths.'],
        linkId: 'workflows',
        linkLabel: 'Explore workflows',
        icon: BookOpen,
      },
      {
        title: 'Whitepaper with architectural traceability',
        summary: 'Current product behavior still maps back to maintained baseline specifications for product, architecture, API, and testing.',
        bullets: ['Baseline specs remain the maintained source of truth behind the whitepaper narrative.', 'Archive pages stay discoverable without being presented as current guidance.'],
        linkId: 'architecture',
        linkLabel: 'Review architecture',
        icon: ShieldCheck,
      },
      {
        title: 'Practical reference entry points',
        summary: 'Teams evaluating integration can still jump straight to CLI, API, build, and test entry points once the reading path has done its job.',
        bullets: ['C++ API and CLI references are grouped under the reference hub.', 'scripts/core remains the supported entry path for build, test, and lint.'],
        linkId: 'reference',
        linkLabel: 'Open reference hub',
        icon: GitBranch,
      },
    ] satisfies Pillar[],
  },
  zh: {
    title: '阅读路径支柱',
    pillars: [
      {
        title: '先证据，后主张',
        summary: '阅读路径把可复核、可限定边界的 benchmark 信号放在叙事附近，而不是藏在营销式表述后面。',
        bullets: ['读取、写出、过滤与统计路径都有维护中的代表性数字。', '研究页面会解释方法学、环境与复现边界。'],
        linkId: 'performance',
        linkLabel: '查看性能证据',
        icon: Cpu,
      },
      {
        title: '先导读，再执行',
        summary: '读者先选对分区根节点，再从上手路线走到常规 filter 流程，中间不必穿过无关内容。',
        bullets: ['导读把叙事、学习、参考与研究四层分开。', '学院卡片直接连接快速开始与工作流路径。'],
        linkId: 'workflows',
        linkLabel: '浏览工作流',
        icon: BookOpen,
      },
      {
        title: '白皮书背后可追溯的架构依据',
        summary: '当前产品行为仍可回链到持续维护的产品、架构、API 与测试 baseline。',
        bullets: ['baseline 规范继续作为白皮书叙事背后的事实来源。', '归档内容可追溯，但不会被展示成当前指南。'],
        linkId: 'architecture',
        linkLabel: '查看架构',
        icon: ShieldCheck,
      },
      {
        title: '直接可用的参考入口',
        summary: '当阅读路径已经帮你判断清楚方向后，评估集成的团队仍可快速跳到 CLI、API、构建与测试入口。',
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
