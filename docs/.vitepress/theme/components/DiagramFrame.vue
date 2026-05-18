<script setup lang="ts">
import { computed, useSlots } from 'vue'
import ArchitectureOverviewDiagram from './diagrams/ArchitectureOverviewDiagram.vue'
import ExecutionModelDiagram from './diagrams/ExecutionModelDiagram.vue'
import ReadingMapDiagram from './diagrams/ReadingMapDiagram.vue'

const props = defineProps<{
  caption?: string
  asset?: 'architecture-overview' | 'execution-model' | 'reading-map'
  locale?: 'en' | 'zh'
}>()
const slots = useSlots()

const diagramAssets = {
  'architecture-overview': ArchitectureOverviewDiagram,
  'execution-model': ExecutionModelDiagram,
  'reading-map': ReadingMapDiagram,
} as const

const diagramComponent = computed(() => (props.asset ? diagramAssets[props.asset] : null))
const hasCaptionSlot = computed(() => Boolean(slots.caption))
</script>

<template>
  <figure class="diagram-frame">
    <div v-if="diagramComponent" class="diagram-frame__asset">
      <component :is="diagramComponent" :locale="props.locale ?? 'en'" />
    </div>
    <slot v-else />
    <figcaption v-if="caption || hasCaptionSlot">
      <slot name="caption">{{ props.caption }}</slot>
    </figcaption>
  </figure>
</template>
