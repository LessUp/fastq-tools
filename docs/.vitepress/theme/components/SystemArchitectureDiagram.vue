<script setup lang="ts">
import { computed } from 'vue'
import type { LocaleKey } from '../content/siteNavigation'

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    title: 'FastQTools system architecture overview',
    description:
      'Input files flow into decompression and record views, then through a parallel processing pipeline where filtering, trimming, and statistics cooperate with output writers and evidence surfaces.',
    intake: 'Input fabric',
    intakeDetail: 'FASTQ streams, gzip members, filesystem ingress',
    io: 'Reader and inflate stage',
    ioDetail: 'chunked reads, libdeflate / zlib-ng, record framing',
    views: 'Batch views',
    viewsDetail: 'string_view records, ownership-safe handoff',
    pipeline: 'Parallel pipeline',
    pipelineDetail: 'oneTBB stages schedule transforms without serial bottlenecks',
    operators: 'QC operators',
    operatorsDetail: 'filtering, trimming, adapters, policy checks',
    statistics: 'Statistics lane',
    statisticsDetail: 'quality summaries, counters, benchmark signals',
    outputs: 'Outputs and reports',
    outputsDetail: 'writers, CLI surfaces, whitepaper evidence',
    evidence: 'Evidence rail',
    evidenceDetail: 'performance, docs, references, operator guidance',
    control: 'Control plane',
    controlDetail: 'CLI, config, reproducible docs',
  },
  zh: {
    title: 'FastQTools 系统架构概览',
    description:
      '输入文件先进入解压与记录视图阶段，再流入并行处理流水线；过滤、修剪、统计与输出写入协同工作，最终把证据与说明暴露给 CLI 和白皮书文档。',
    intake: '输入织构',
    intakeDetail: 'FASTQ 流、gzip 成员与文件系统入口',
    io: '读取与解压阶段',
    ioDetail: '分块读取、libdeflate / zlib-ng、记录分帧',
    views: '批次视图',
    viewsDetail: 'string_view 记录与安全所有权移交',
    pipeline: '并行流水线',
    pipelineDetail: 'oneTBB 阶段负责调度，不把热点路径拉回串行',
    operators: 'QC 算子',
    operatorsDetail: '过滤、修剪、接头处理与策略检查',
    statistics: '统计通道',
    statisticsDetail: '质量汇总、计数器与 benchmark 信号',
    outputs: '输出与报告',
    outputsDetail: '写入器、CLI 暴露面与白皮书证据',
    evidence: '证据轨道',
    evidenceDetail: '性能、文档、引用与操作指南',
    control: '控制平面',
    controlDetail: 'CLI、配置与可复现实验文档',
  },
} as const

const content = computed(() => copy[props.locale])
const titleId = computed(() => `system-architecture-title-${props.locale}`)
const descriptionId = computed(() => `system-architecture-description-${props.locale}`)
</script>

<template>
  <svg
    class="system-architecture-diagram"
    viewBox="0 0 920 520"
    role="img"
    :aria-labelledby="`${titleId} ${descriptionId}`"
  >
    <title :id="titleId">{{ content.title }}</title>
    <desc :id="descriptionId">{{ content.description }}</desc>

    <rect class="diagram-surface" x="24" y="24" width="872" height="472" rx="32" />

    <rect class="diagram-panel diagram-panel--signal" x="44" y="84" width="176" height="140" rx="24" />
    <text class="diagram-label" x="70" y="128">{{ content.intake }}</text>
    <text class="diagram-detail" x="70" y="154">{{ content.intakeDetail }}</text>

    <rect class="diagram-panel" x="260" y="72" width="196" height="156" rx="24" />
    <text class="diagram-label" x="286" y="116">{{ content.io }}</text>
    <text class="diagram-detail" x="286" y="142">{{ content.ioDetail }}</text>

    <rect class="diagram-panel" x="260" y="264" width="196" height="136" rx="24" />
    <text class="diagram-label" x="286" y="308">{{ content.views }}</text>
    <text class="diagram-detail" x="286" y="334">{{ content.viewsDetail }}</text>

    <rect class="diagram-panel diagram-panel--accent" x="500" y="48" width="232" height="352" rx="28" />
    <text class="diagram-label" x="530" y="94">{{ content.pipeline }}</text>
    <text class="diagram-detail" x="530" y="120">{{ content.pipelineDetail }}</text>

    <rect class="diagram-chip" x="530" y="150" width="172" height="72" rx="18" />
    <text class="diagram-chip-label" x="554" y="188">{{ content.operators }}</text>
    <text class="diagram-chip-detail" x="554" y="210">{{ content.operatorsDetail }}</text>

    <rect class="diagram-chip" x="530" y="246" width="172" height="82" rx="18" />
    <text class="diagram-chip-label" x="554" y="284">{{ content.statistics }}</text>
    <text class="diagram-chip-detail" x="554" y="306">{{ content.statisticsDetail }}</text>

    <rect class="diagram-panel diagram-panel--signal" x="756" y="84" width="116" height="140" rx="22" />
    <text class="diagram-label" x="780" y="128">{{ content.outputs }}</text>
    <text class="diagram-detail" x="780" y="154">{{ content.outputsDetail }}</text>

    <rect class="diagram-panel diagram-panel--signal" x="756" y="260" width="116" height="140" rx="22" />
    <text class="diagram-label" x="780" y="304">{{ content.evidence }}</text>
    <text class="diagram-detail" x="780" y="330">{{ content.evidenceDetail }}</text>

    <rect class="diagram-band" x="104" y="432" width="712" height="44" rx="18" />
    <text class="diagram-band-label" x="132" y="460">{{ content.control }}</text>
    <text class="diagram-band-detail" x="286" y="460">{{ content.controlDetail }}</text>

    <path class="diagram-flow" d="M220 154H260" />
    <path class="diagram-flow" d="M356 228V264" />
    <path class="diagram-flow diagram-flow--accent" d="M456 150H500" />
    <path class="diagram-flow diagram-flow--accent" d="M456 332H500" />
    <path class="diagram-flow diagram-flow--signal" d="M732 186H756" />
    <path class="diagram-flow diagram-flow--signal" d="M732 290H756" />
    <path class="diagram-flow" d="M356 400V432" />
    <path class="diagram-flow diagram-flow--accent" d="M618 400V432" />
    <path class="diagram-flow diagram-flow--signal" d="M814 400V432" />
  </svg>
</template>

<style scoped>
.system-architecture-diagram {
  display: block;
  width: 100%;
  min-width: min(42rem, 100%);
  color: var(--fq-home-diagram-text);
}

.diagram-surface {
  fill: var(--fq-home-diagram-surface);
  stroke: var(--fq-home-diagram-stroke);
  stroke-width: 1.5;
}

.diagram-panel {
  fill: var(--fq-home-diagram-panel);
  stroke: var(--fq-home-diagram-stroke);
  stroke-width: 1.5;
}

.diagram-panel--accent {
  fill: var(--fq-home-diagram-accent-soft);
  stroke: var(--fq-home-diagram-accent);
}

.diagram-panel--signal {
  fill: var(--fq-home-diagram-signal-soft);
  stroke: var(--fq-home-diagram-signal);
}

.diagram-chip {
  fill: var(--fq-home-diagram-chip);
  stroke: var(--fq-home-diagram-stroke);
  stroke-width: 1.2;
}

.diagram-band {
  fill: var(--fq-home-diagram-band);
  stroke: var(--fq-home-diagram-stroke);
  stroke-width: 1.2;
}

.diagram-label,
.diagram-chip-label,
.diagram-band-label {
  fill: var(--fq-home-diagram-text);
  font-size: 15px;
  font-weight: 700;
}

.diagram-detail,
.diagram-chip-detail,
.diagram-band-detail {
  fill: var(--fq-home-diagram-muted);
  font-size: 11.5px;
}

.diagram-flow {
  fill: none;
  stroke: var(--fq-home-diagram-stroke);
  stroke-linecap: round;
  stroke-width: 2.4;
}

.diagram-flow--accent {
  stroke: var(--fq-home-diagram-accent);
}

.diagram-flow--signal {
  stroke: var(--fq-home-diagram-signal);
  stroke-dasharray: 8 8;
}
</style>
