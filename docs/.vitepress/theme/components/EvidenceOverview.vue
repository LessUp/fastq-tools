<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type EvidenceCard = {
  label: string
  value: string
  detail: string
  linkId: SharedLinkId
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'Evidence overview',
    title: 'Operational claims are paired with the trail needed to defend them.',
    summary:
      'Readers should not have to guess where throughput numbers, engineering constraints, or maintenance posture come from. This section turns those claims into explicit evidence channels.',
    cards: [
      {
        label: 'Throughput class',
        value: 'Up to 1.7M reads/s',
        detail: 'Performance sections and benchmark notes capture the context around the headline number.',
        linkId: 'performance',
      },
      {
        label: 'Execution discipline',
        value: 'oneTBB + zero-copy',
        detail: 'Architecture and workflow chapters explain where concurrency matters and where ownership remains explicit.',
        linkId: 'architecture',
      },
      {
        label: 'Verification surface',
        value: 'Tests, docs, references',
        detail: 'Reference and research routes keep reproducibility, contribution norms, and supporting material visible.',
        linkId: 'researchNav',
      },
    ] satisfies EvidenceCard[],
  },
  zh: {
    eyebrow: '证据概览',
    title: '所有运行主张都要附带可追溯的证据路径。',
    summary:
      '读者不该猜测吞吐数字、工程约束或维护姿态从何而来。本节把这些主张压缩成明确的证据通道。',
    cards: [
      {
        label: '吞吐等级',
        value: '最高约 170 万 reads/s',
        detail: '性能章节与 benchmark 说明会交代 headline 数字的上下文。',
        linkId: 'performance',
      },
      {
        label: '执行纪律',
        value: 'oneTBB + 零拷贝',
        detail: '架构与工作流章节解释并发真正发生在哪里，以及所有权如何保持清晰。',
        linkId: 'architecture',
      },
      {
        label: '验证表面',
        value: '测试、文档、引用',
        detail: '参考与研究路径持续暴露可复现性、协作规范与补充材料。',
        linkId: 'researchNav',
      },
    ] satisfies EvidenceCard[],
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (linkId: SharedLinkId) => withBase(getSharedLinkHref(props.locale, linkId))
</script>

<template>
  <section class="home-whitepaper home-section">
    <div class="section-header">
      <p class="section-kicker">{{ content.eyebrow }}</p>
      <h2 class="section-heading">{{ content.title }}</h2>
      <p class="section-lead">{{ content.summary }}</p>
    </div>

    <div class="evidence-grid">
      <article v-for="card in content.cards" :key="card.label" class="paper-card evidence-card">
        <p class="evidence-label">{{ card.label }}</p>
        <h3 class="evidence-value">{{ card.value }}</h3>
        <p>{{ card.detail }}</p>
        <a class="paper-link tertiary evidence-link" :href="resolve(card.linkId)">
          <span>{{ props.locale === 'en' ? 'Open source section' : '打开对应章节' }}</span>
          <ArrowRight :size="16" />
        </a>
      </article>
    </div>
  </section>
</template>
