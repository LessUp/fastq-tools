<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type SectionKey = 'orientation' | 'whitepaper' | 'academy' | 'reference' | 'research'

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
    orientation: {
      eyebrow: 'Orientation',
      title: 'Pick the right entry point before you read deeply.',
      summary: 'Orientation is the shortest route into the new reading path. Use it to understand how whitepaper, academy, reference, and research fit together.',
      hubLinkId: 'orientationNav',
      hubLinkLabel: 'Open orientation',
      cards: [
        {
          tag: 'Map',
          title: 'Use the knowledge map first',
          summary: 'Choose the best section root for your current question before you commit to a long reading session.',
          linkId: 'knowledgeMap',
          linkLabel: 'Open knowledge map',
        },
        {
          tag: 'Story',
          title: 'Continue into the whitepaper',
          summary: 'Move into positioning, architecture, and performance claims once you know you need the narrative case.',
          linkId: 'whitepaperNav',
          linkLabel: 'Read the whitepaper',
        },
        {
          tag: 'Need concrete details?',
          title: 'Jump straight to reference',
          summary: 'When you already know the task, go directly to guides, API pages, and developer material.',
          linkId: 'referenceNav',
          linkLabel: 'Open reference',
        },
      ],
    },
    whitepaper: {
      eyebrow: 'Whitepaper',
      title: 'Review the technical whitepaper before making adoption claims.',
      summary: 'The whitepaper section frames why FastQTools exists, what constraints shaped it, and how the evidence should be read.',
      hubLinkId: 'whitepaperNav',
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
    academy: {
      eyebrow: 'Academy',
      title: 'Use guided learning paths before diving into full manuals.',
      summary: 'Academy translates the narrative into action: onboarding, repeatable workflows, and the shortest route to execution-focused docs.',
      hubLinkId: 'algorithms',
      hubLinkLabel: 'Open algorithms',
      cards: [
        {
          tag: 'Path',
          title: 'Choose a workflow path',
          summary: 'See whether you are evaluating the tool, operating routine QC work, or contributing code.',
          linkId: 'workflows',
          linkLabel: 'Open workflows',
        },
        {
          tag: 'Onboarding',
          title: 'Run the getting-started route',
          summary: 'Move from install and build to the first successful stat command without browsing unrelated material.',
          linkId: 'gettingStarted',
          linkLabel: 'Open getting started',
        },
        {
          tag: 'Deep dive',
          title: 'Drop into CLI reference when needed',
          summary: 'Once you know your workflow, move into exact flags, configuration details, and command behavior.',
          linkId: 'cliReference',
          linkLabel: 'Open CLI reference',
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
      title: 'Gather supporting evidence, context, and maintenance material.',
      summary: 'Research keeps benchmark reports, resource hubs, contribution context, and archive material nearby without interrupting the main reading path.',
      hubLinkId: 'researchNav',
      hubLinkLabel: 'Open research',
      cards: [
        {
          tag: 'Benchmarks',
          title: 'Inspect the benchmark report',
          summary: 'Use the detailed report when you need methodology, environment notes, or a closer look at representative runs.',
          linkId: 'benchmarkReport',
          linkLabel: 'Open benchmark report',
        },
        {
          tag: 'Resources',
          title: 'Collect adjacent references',
          summary: 'Find release notes, repository links, collaboration material, and supporting external context in one place.',
          linkId: 'resourcesOverview',
          linkLabel: 'Open resources',
        },
        {
          tag: 'Contribute',
          title: 'Prepare for active participation',
          summary: 'Move into contribution and maintenance documents when evaluation turns into implementation or collaboration.',
          linkId: 'contributing',
          linkLabel: 'Read contributing',
        },
      ],
    },
  },
  zh: {
    orientation: {
      eyebrow: '导读',
      title: '先选对入口，再决定要深入阅读哪一层。',
      summary: '导读层是新阅读路径的最短入口，用来快速说明白皮书、学院、参考、研究之间的关系。',
      hubLinkId: 'orientationNav',
      hubLinkLabel: '进入导读',
      cards: [
        {
          tag: '地图',
          title: '先看知识地图',
          summary: '在投入较长阅读时间前，先确认当前问题最适合落在哪个分区根节点。',
          linkId: 'knowledgeMap',
          linkLabel: '打开知识地图',
        },
        {
          tag: '叙事',
          title: '继续进入白皮书',
          summary: '当你需要了解定位、架构与性能论证时，再进入白皮书层阅读完整叙事。',
          linkId: 'whitepaperNav',
          linkLabel: '阅读白皮书',
        },
        {
          tag: '已经有明确任务？',
          title: '直接跳到参考层',
          summary: '如果你已经知道自己要找什么，可以直接进入指南、API 与开发资料。',
          linkId: 'referenceNav',
          linkLabel: '打开参考导航',
        },
      ],
    },
    whitepaper: {
      eyebrow: '白皮书',
      title: '在做采用判断前，先读完这组技术白皮书材料。',
      summary: '白皮书层负责解释 FastQTools 为什么存在、受哪些约束影响，以及证据应该如何理解。',
      hubLinkId: 'whitepaperNav',
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
    academy: {
      eyebrow: '学院',
      title: '先走学习路径，再进入完整手册。',
      summary: '学院层把叙事转成行动：如何上手、如何形成可重复工作流、以及何时进入执行细节文档。',
      hubLinkId: 'algorithms',
      hubLinkLabel: '进入算法',
      cards: [
        {
          tag: '路径',
          title: '先选工作流路径',
          summary: '先判断你是处在采用评估、常规质控执行，还是贡献开发的阶段。',
          linkId: 'workflows',
          linkLabel: '打开工作流',
        },
        {
          tag: '上手',
          title: '执行快速开始路线',
          summary: '从依赖安装与构建走到第一次成功执行 stat，中间不需要浏览无关材料。',
          linkId: 'gettingStarted',
          linkLabel: '打开快速开始',
        },
        {
          tag: '深入',
          title: '需要时再进入 CLI 参考',
          summary: '当你已经明确要执行的流程，再查看精确参数、配置细节与命令行为。',
          linkId: 'cliReference',
          linkLabel: '打开 CLI 参考',
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
      title: '集中查看支撑证据、外部上下文与维护材料。',
      summary: '研究层把 benchmark 报告、资源入口、贡献上下文与归档材料放在一起，但不会打断主阅读路径。',
      hubLinkId: 'researchNav',
      hubLinkLabel: '进入研究',
      cards: [
        {
          tag: '基准',
          title: '查看 benchmark 报告',
          summary: '当你需要方法学、环境说明或更细的代表性运行数据时，直接进入完整报告。',
          linkId: 'benchmarkReport',
          linkLabel: '打开 benchmark 报告',
        },
        {
          tag: '资源',
          title: '收集站内外参考入口',
          summary: '发布记录、仓库链接、协作说明与外围参考资料在这里集中整理。',
          linkId: 'resourcesOverview',
          linkLabel: '打开资源中心',
        },
        {
          tag: '参与',
          title: '为实际协作做准备',
          summary: '当评估阶段转向实施或协作时，继续进入贡献与维护文档。',
          linkId: 'contributing',
          linkLabel: '阅读参与贡献',
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
