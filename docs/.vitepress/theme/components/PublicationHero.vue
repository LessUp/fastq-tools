<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight, FileText, Gauge, Network } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type Action = {
  label: string
  linkId: SharedLinkId
  variant: 'primary' | 'secondary'
}

type Signal = {
  title: string
  detail: string
  icon: typeof FileText
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'FastQTools technical whitepaper',
    title: 'A publication-grade FASTQ systems brief for adoption review, design transfer, and evidence audit.',
    summary:
      'FastQTools is presented here as a bounded quality-control engine: zero-copy I/O, oneTBB execution, and maintained benchmark policy are treated as one technical narrative rather than as separate marketing claims.',
    actions: [
      { label: 'Read the whitepaper', linkId: 'whitepaperNav', variant: 'primary' },
      { label: 'Inspect architecture', linkId: 'architecture', variant: 'secondary' },
      { label: 'Open research appendix', linkId: 'researchNav', variant: 'secondary' },
    ] satisfies Action[],
    signals: [
      {
        title: 'Boundary-first framing',
        detail: 'The landing page opens with what FastQTools is allowed to claim, not with an undifferentiated feature wall.',
        icon: FileText,
      },
      {
        title: 'System model in view',
        detail: 'Execution stages, memory discipline, and evidence surfaces stay visible from the first scroll.',
        icon: Network,
      },
      {
        title: 'Proof adjacent to claims',
        detail: 'Performance and reference routes remain one hop away, so technical review can stay auditable.',
        icon: Gauge,
      },
    ] satisfies Signal[],
  },
  zh: {
    eyebrow: 'FastQTools 技术白皮书',
    title: '面向采用评审、设计移交与证据复核的 FASTQ 系统级白皮书首页。',
    summary:
      '这里的 FastQTools 被组织成一个受控的质控引擎：零拷贝 I/O、oneTBB 执行模型与 benchmark 维护政策被写进同一条技术叙事，而不是拆散成零散卖点。',
    actions: [
      { label: '阅读白皮书', linkId: 'whitepaperNav', variant: 'primary' },
      { label: '审查架构', linkId: 'architecture', variant: 'secondary' },
      { label: '打开研究附录', linkId: 'researchNav', variant: 'secondary' },
    ] satisfies Action[],
    signals: [
      {
        title: '以边界为先的开场',
        detail: '首页先说明 FastQTools 今天能被允许主张什么，而不是先堆一整墙功能名词。',
        icon: FileText,
      },
      {
        title: '系统模型始终在场',
        detail: '执行阶段、内存约束与证据表面在第一屏就进入读者视野。',
        icon: Network,
      },
      {
        title: '主张旁边就是证据',
        detail: '性能与参考路径始终保持一步可达，让技术评审可以直接追溯。',
        icon: Gauge,
      },
    ] satisfies Signal[],
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (linkId: SharedLinkId) => withBase(getSharedLinkHref(props.locale, linkId))
</script>

<template>
  <section class="home-whitepaper hero-whitepaper publication-hero">
    <div class="publication-hero-grid">
      <div class="hero-copy">
        <p class="section-kicker">{{ content.eyebrow }}</p>
        <h1 class="hero-title">{{ content.title }}</h1>
        <p class="section-lead hero-summary">{{ content.summary }}</p>

        <div class="section-actions">
          <a
            v-for="action in content.actions"
            :key="action.linkId"
            :class="['paper-link', action.variant]"
            :href="resolve(action.linkId)"
          >
            <span>{{ action.label }}</span>
            <ArrowRight :size="16" />
          </a>
        </div>
      </div>

      <aside class="paper-card hero-brief publication-abstract">
        <p class="section-kicker">{{ props.locale === 'en' ? 'Review frame' : '评审框架' }}</p>
        <h2 class="section-heading brief-heading">
          {{
            props.locale === 'en'
              ? 'Why the homepage now reads like a systems paper'
              : '为什么首页现在像一篇系统白皮书'
          }}
        </h2>

        <div class="publication-proof-grid">
          <article v-for="signal in content.signals" :key="signal.title" class="paper-metric">
            <div class="metric-icon">
              <component :is="signal.icon" :size="18" />
            </div>
            <div>
              <h3>{{ signal.title }}</h3>
              <p>{{ signal.detail }}</p>
            </div>
          </article>
        </div>
      </aside>
    </div>
  </section>
</template>
