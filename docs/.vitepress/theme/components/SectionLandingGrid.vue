<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type SectionKey = 'whitepaper' | 'reference' | 'research'

type SectionCard = {
  tag: string
  title: string
  summary: string
  linkId: SharedLinkId
  linkLabel: string
}

type SectionCopy = {
  eyebrow: string
  title: string
  summary: string
  hubLinkId: SharedLinkId
  hubLinkLabel: string
  cards: SectionCard[]
}

const props = defineProps<{ locale: LocaleKey; section: SectionKey }>()

const copy: Record<LocaleKey, Record<SectionKey, SectionCopy>> = {
  en: {
    whitepaper: {
      eyebrow: 'Whitepaper',
      title: 'Review the technical whitepaper before making adoption claims.',
      summary: 'The whitepaper section frames why FastQTools exists, what constraints shaped it, and how the evidence should be read.',
      hubLinkId: 'whitepaper',
      hubLinkLabel: 'Open whitepaper',
      cards: [
        {
          tag: 'Positioning',
          title: 'Start with why FastQTools',
          summary: 'Understand the product boundary, intended use, and the specific problems the project is trying to solve.',
          linkId: 'why',
          linkLabel: 'Read positioning',
        },
        {
          tag: 'Architecture',
          title: 'Trace the design rationale',
          summary: 'Review the architectural decisions that support zero-copy IO, constrained workflows, and maintainable quality controls.',
          linkId: 'architecture',
          linkLabel: 'Read architecture',
        },
        {
          tag: 'Evidence',
          title: 'Validate the performance story',
          summary: 'Read benchmark-backed claims before repeating them in adoption documents or technical reviews.',
          linkId: 'performance',
          linkLabel: 'Review performance',
        },
      ],
    },
    reference: {
      eyebrow: 'Reference',
      title: 'Move directly to lookup material when the task is already clear.',
      summary: 'Reference is the operational map for command syntax, APIs, developer internals, and the adjacent materials that support concrete execution.',
      hubLinkId: 'referenceNav',
      hubLinkLabel: 'Open reference',
      cards: [
        {
          tag: 'CLI',
          title: 'Find exact command behavior',
          summary: 'Jump into flags, subcommands, and workflow fragments when you already know which execution detail you need.',
          linkId: 'cliReference',
          linkLabel: 'Open CLI reference',
        },
        {
          tag: 'API',
          title: 'Inspect public interfaces',
          summary: 'Move from command usage into API surface and module-level boundaries when library integration matters.',
          linkId: 'apiOverview',
          linkLabel: 'Open API overview',
        },
        {
          tag: 'Internals',
          title: 'Trace developer-facing design',
          summary: 'Use developer architecture material when the task requires build, testing, or implementation-level reasoning.',
          linkId: 'devArchitecture',
          linkLabel: 'Open developer architecture',
        },
      ],
    },
    research: {
      eyebrow: 'Research',
      title: 'Gather supporting evidence, adjacent context, and design history.',
      summary: 'Research now foregrounds the bibliography, related-project comparisons, and evolution notes that back the maintained whitepaper narrative.',
      hubLinkId: 'researchNav',
      hubLinkLabel: 'Open research',
      cards: [
        {
          tag: 'Sources',
          title: 'Trace the bibliography',
          summary: 'Use the bibliography to validate format claims, benchmark language, and the RFC trail behind the maintained narrative.',
          linkId: 'bibliography',
          linkLabel: 'Open bibliography',
        },
        {
          tag: 'Comparisons',
          title: 'Compare related projects',
          summary: 'Open adjacent-project context when you need to explain how FastQTools differs from FastQC, fastp, Cutadapt, and seqtk.',
          linkId: 'relatedProjects',
          linkLabel: 'Open related projects',
        },
        {
          tag: 'History',
          title: 'Review evolution notes',
          summary: 'Read the design and policy history when the question is why the current architecture and benchmark boundaries were preserved.',
          linkId: 'evolutionNotes',
          linkLabel: 'Open evolution notes',
        },
      ],
    },
  },
  zh: {
    whitepaper: {
      eyebrow: '白皮书',
      title: '在做采用判断前，先读完这组技术白皮书材料。',
      summary: '白皮书层负责解释 FastQTools 为什么存在、受哪些约束影响，以及证据应该如何理解。',
      hubLinkId: 'whitepaper',
      hubLinkLabel: '进入白皮书',
      cards: [
        {
          tag: '定位',
          title: '先看为什么选择 FastQTools',
          summary: '先理解产品边界、适用场景，以及项目真正要解决的问题。',
          linkId: 'why',
          linkLabel: '阅读定位说明',
        },
        {
          tag: '架构',
          title: '追踪设计决策',
          summary: '阅读支撑零拷贝 IO、受控工作流与可维护质量控制的架构背景。',
          linkId: 'architecture',
          linkLabel: '阅读架构',
        },
        {
          tag: '证据',
          title: '核对白皮书中的性能论证',
          summary: '在把吞吐或采用结论写进评审材料前，先回到性能证据页面逐项确认。',
          linkId: 'performance',
          linkLabel: '查看性能总览',
        },
      ],
    },
    reference: {
      eyebrow: '参考',
      title: '当任务已经明确时，直接进入查阅材料。',
      summary: '参考层是命令语法、API、开发者内部设计与周边资料的执行地图，用来支持明确问题下的快速落点。',
      hubLinkId: 'referenceNav',
      hubLinkLabel: '进入参考导航',
      cards: [
        {
          tag: 'CLI',
          title: '查精确命令行为',
          summary: '当你已经知道要解决什么执行问题时，直接进入参数、子命令与工作流片段。',
          linkId: 'cliReference',
          linkLabel: '打开 CLI 参考',
        },
        {
          tag: 'API',
          title: '查看公共接口',
          summary: '当任务涉及库集成或模块边界时，从命令使用切换到 API 总览与模块接口。',
          linkId: 'apiOverview',
          linkLabel: '打开 API 概览',
        },
        {
          tag: '内部设计',
          title: '追踪开发者侧设计',
          summary: '当问题进入构建、测试或实现层推理时，继续进入开发者架构材料。',
          linkId: 'devArchitecture',
          linkLabel: '打开开发者架构设计',
        },
      ],
    },
    research: {
      eyebrow: '研究',
      title: '集中查看来源、对照语境与设计历史。',
      summary: '研究层现在优先暴露书目、相关项目与演进说明，用来支撑维护中的白皮书叙事。',
      hubLinkId: 'researchNav',
      hubLinkLabel: '进入研究',
      cards: [
        {
          tag: '来源',
          title: '追踪书目',
          summary: '当你需要格式背景、benchmark 术语来源或 RFC 证据链时，先进入书目。',
          linkId: 'bibliography',
          linkLabel: '打开书目',
        },
        {
          tag: '对照',
          title: '查看相关项目',
          summary: '当你需要解释 FastQTools 与 FastQC、fastp、Cutadapt、seqtk 的边界差异时，从这里进入。',
          linkId: 'relatedProjects',
          linkLabel: '打开相关项目',
        },
        {
          tag: '历史',
          title: '阅读演进说明',
          summary: '当问题变成“为什么今天会维护这条边界”时，继续进入设计与政策的演进记录。',
          linkId: 'evolutionNotes',
          linkLabel: '打开演进说明',
        },
      ],
    },
  },
}

const content = computed(() => copy[props.locale][props.section])
const resolve = (id: SharedLinkId) => withBase(getSharedLinkHref(props.locale, id))
</script>

<template>
  <section class="section-landing-grid">
    <header class="section-header">
      <div>
        <p class="eyebrow">{{ content.eyebrow }}</p>
        <h2>{{ content.title }}</h2>
        <p class="summary">{{ content.summary }}</p>
      </div>
      <a class="section-hub-link" :href="resolve(content.hubLinkId)">
        <span>{{ content.hubLinkLabel }}</span>
        <ArrowRight :size="16" />
      </a>
    </header>

    <div class="section-grid">
      <article v-for="card in content.cards" :key="card.title" class="section-card">
        <p class="tag">{{ card.tag }}</p>
        <h3>{{ card.title }}</h3>
        <p>{{ card.summary }}</p>
        <a :href="resolve(card.linkId)">
          <span>{{ card.linkLabel }}</span>
          <ArrowRight :size="16" />
        </a>
      </article>
    </div>
  </section>
</template>

<style scoped>
.section-landing-grid {
  display: grid;
  gap: 1rem;
  margin: 0 0 2rem;
}

.section-header {
  display: flex;
  align-items: end;
  justify-content: space-between;
  gap: 1rem;
  padding: 1.15rem 1.2rem 0;
}

.eyebrow {
  margin: 0 0 0.45rem;
  color: var(--vp-c-brand-1);
  font-size: 0.8rem;
  font-weight: 700;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}

.section-header h2 {
  margin: 0;
  border: 0;
  padding: 0;
  font-size: 1.45rem;
}

.section-header h2::after {
  display: none;
}

.summary {
  margin: 0.7rem 0 0;
  max-width: 46rem;
  color: var(--vp-c-text-2);
}

.section-hub-link,
.section-card a {
  display: inline-flex;
  align-items: center;
  gap: 0.45rem;
  min-height: 2.6rem;
  padding: 0.65rem 0.9rem;
  border: 1px solid var(--vp-c-border);
  border-radius: 999px;
  background: color-mix(in srgb, var(--vp-c-bg-elv) 84%, transparent);
  color: var(--vp-c-text-1);
  font-weight: 700;
  transition: border-color var(--transition-normal), background var(--transition-normal);
}

.section-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 1rem;
}

.section-card {
  display: grid;
  gap: 0.8rem;
  padding: 1rem;
  border: 1px solid var(--vp-c-border);
  border-radius: var(--radius-lg);
  background: linear-gradient(180deg, color-mix(in srgb, var(--vp-c-bg-elv) 96%, transparent), color-mix(in srgb, var(--vp-c-bg-soft) 44%, transparent));
  box-shadow: var(--fq-shadow-card);
}

.tag {
  margin: 0;
  color: var(--vp-c-brand-1);
  font-size: 0.78rem;
  font-weight: 700;
  letter-spacing: 0.06em;
  text-transform: uppercase;
}

.section-card h3 {
  margin: 0;
  color: var(--vp-c-text-1);
  font-size: 1.03rem;
}

.section-card p:last-of-type {
  margin: 0;
  color: var(--vp-c-text-2);
}

.section-hub-link:hover,
.section-card a:hover {
  border-color: var(--fq-c-border-strong);
  background: color-mix(in srgb, var(--fq-c-brand-soft) 80%, var(--vp-c-bg-elv));
}

@media (max-width: 960px) {
  .section-header {
    align-items: start;
    flex-direction: column;
    padding-inline: 0;
  }

  .section-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}

@media (max-width: 640px) {
  .section-grid {
    grid-template-columns: 1fr;
  }

  .section-hub-link,
  .section-card a {
    width: 100%;
    justify-content: center;
  }
}
</style>
