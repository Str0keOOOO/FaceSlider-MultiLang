<script setup lang="ts">
import { ref, reactive, onMounted, onUnmounted, defineProps } from 'vue'
import type { CSSProperties } from 'vue'

const containerRef = ref<HTMLElement>(null!)
const btnHappyRef = ref<HTMLElement>(null!)
const btnUnhappyRef = ref<HTMLElement>(null!)

interface Props {
  maxUnhappyCount?: number
  animationSpeed?: number
  normalFace?: { happiness: number; derp: number; px: number; py: number }
  normalUi?: { btnHappyText: string; btnUnhappyText: string; titleText: string; subtitleText: string }
}

interface Config {
  maxUnhappyCount: number;
  animationSpeed: number;
  states: {
    normal: {
      face: {
        happiness: number;
        derp: number;
        px: number;
        py: number;
      };
      ui: {
        btnHappyText: string;
        btnUnhappyText: string;
        titleText: string;
        subtitleText: string;
      };
      uiStyle: {
        btnHappyVisibility: CSSProperties['visibility'];
        btnUnhappyVisibility: CSSProperties['visibility'];
        btnUnhappyPosition: CSSProperties['position'];
        btnUnhappyLeft: CSSProperties['left'];
        btnUnhappyTop: CSSProperties['top'];
        btnHappyTransform: CSSProperties['transform'];
      };
    };
    happy: {
      face: {
        happiness: number;
        derp: number;
        px: number;
        py: number;
      };
      ui: {
        btnHappyText: string;
        btnUnhappyText: string;
        titleText: string;
        subtitleText: string;
      };
      uiStyle: {
        btnHappyVisibility: CSSProperties['visibility'];
        btnUnhappyVisibility: CSSProperties['visibility']
        btnUnhappyPosition: CSSProperties['position'];
        btnUnhappyLeft: CSSProperties['left'];
        btnUnhappyTop: CSSProperties['top'];
        btnHappyTransform: CSSProperties['transform'];
      };
    };
    unhappy: {
      face: {
        happiness: number;
        derp: number;
        px: number;
        py: number;
      };
      ui: {
        btnHappyText: string;
        btnUnhappyText: string;
        titleText: string;
        subtitleText: string;
      };
      uiStyle: {
        btnHappyVisibility: CSSProperties['visibility'];
        btnUnhappyVisibility: CSSProperties['visibility'];
        btnUnhappyPosition: CSSProperties['position'];
        btnUnhappyLeft: CSSProperties['left'];
        btnUnhappyTop: CSSProperties['top'];
        btnHappyTransform: CSSProperties['transform'];
      };
    };
  };
}

const props = defineProps<Props>()

const config: Config = {
  maxUnhappyCount: props.maxUnhappyCount || 3,
  animationSpeed: props.animationSpeed || 0.1,
  states: {
    normal: {
      face: props.normalFace || { happiness: 0.9, derp: 1, px: 0.5, py: 0.5 },
      ui: props.normalUi || {
        btnHappyText: "取消",
        btnUnhappyText: "卸载",
        titleText: "你确定要卸载吗",
        subtitleText: "希望不要卸载"
      },
      uiStyle: {
        btnHappyVisibility: "visible",
        btnUnhappyVisibility: "visible",
        btnUnhappyPosition: "static",
        btnUnhappyLeft: "0",
        btnUnhappyTop: "0",
        btnHappyTransform: "scale(1)"
      }
    },
    happy: {
      face: { happiness: 1, derp: 0, px: 0.5, py: 0.5 },
      ui: {
        btnHappyText: "返回",
        btnUnhappyText: "",
        titleText: "已取消",
        subtitleText: "感谢您继续使用本插件"
      },
      uiStyle: {
        btnHappyVisibility: "visible",
        btnUnhappyVisibility: "hidden",
        btnUnhappyPosition: "static",
        btnUnhappyLeft: "0",
        btnUnhappyTop: "0",
        btnHappyTransform: "scale(1)"
      }
    },
    unhappy: {
      face: { happiness: 0.2, derp: 0, px: 0.5, py: 0.5 },
      ui: {
        btnHappyText: "",
        btnUnhappyText: "返回",
        titleText: "已删除",
        subtitleText: "感谢您使用本插件"
      },
      uiStyle: {
        btnHappyVisibility: "hidden",
        btnUnhappyVisibility: "visible",
        btnUnhappyPosition: "static",
        btnUnhappyLeft: "0",
        btnUnhappyTop: "0",
        btnHappyTransform: "scale(1)"
      }
    }
  }
}

const state = reactive({
  rejectCount: 0,
  animationId: null as number | null,
  ui: { ...config.states.normal.ui },
  uiStyle: { ...config.states.normal.uiStyle },
  currentface: { ...config.states.normal.face },
  targetface: { ...config.states.normal.face }
})


function updateFace() {
  for (const key of Object.keys(state.targetface) as (keyof Config['states']['normal']['face'])[]) {
    if (state.targetface[key] === state.currentface[key]) continue
    if (Math.abs(state.targetface[key] - state.currentface[key]) < 0.01) {
      state.currentface[key] = state.targetface[key]
    } else {
      state.currentface[key] += (state.targetface[key] - state.currentface[key]) * config.animationSpeed
    }
  }
  state.animationId = requestAnimationFrame(updateFace)
}

function happy() {
  if (state.animationId) {
    state.ui = { ...config.states.happy.ui }
    state.uiStyle = { ...config.states.happy.uiStyle }
    cancelAnimationFrame(state.animationId)
    state.animationId = null
    state.currentface = { ...config.states.happy.face }
  } else {
    state.rejectCount = 0
    state.ui = { ...config.states.normal.ui }
    state.uiStyle = { ...config.states.normal.uiStyle }
    updateFace()
  }
}

function unHappy() {
  if (state.animationId) {
    state.rejectCount++
    if (state.rejectCount >= config.maxUnhappyCount) {
      state.ui = { ...config.states.happy.ui }
      state.uiStyle = { ...config.states.happy.uiStyle }
      cancelAnimationFrame(state.animationId)
      state.animationId = null
      state.currentface = { ...config.states.unhappy.face }
    } else {
      state.uiStyle.btnUnhappyPosition = 'absolute'
      state.uiStyle.btnUnhappyLeft = `${Math.random() * 80}%`
      state.uiStyle.btnUnhappyTop = `${Math.random() * 80}%`
      state.targetface.happiness = Math.max(0.1, state.targetface.happiness - 0.1)
      state.uiStyle.btnHappyTransform = `scale(${1 + state.rejectCount * 0.1})`
    }
  } else {
    state.rejectCount = 0
    state.ui = { ...config.states.normal.ui }
    state.uiStyle = { ...config.states.normal.uiStyle }
    updateFace()
  }
}

function handleMouseMove({ clientX: x, clientY: y }: MouseEvent) {

  const unhappyRect = containerRef.value.getBoundingClientRect()
  const happyRect = btnHappyRef.value.getBoundingClientRect()
  const containerRect = containerRef.value.getBoundingClientRect()

  const dx1 = x - (unhappyRect.x + unhappyRect.width * 0.5)
  const dy1 = y - (unhappyRect.y + unhappyRect.height * 0.5)
  const dx2 = x - (happyRect.x + happyRect.width * 0.5)
  const dy2 = y - (happyRect.y + happyRect.height * 0.5)

  const px = (x - containerRect.x) / containerRect.width
  const py = (y - containerRect.y) / containerRect.height

  const distUnhappy = Math.sqrt(dx1 * dx1 + dy1 * dy1)
  const distHappy = Math.sqrt(dx2 * dx2 + dy2 * dy2)
  const happiness = Math.pow(distUnhappy / (distHappy + distUnhappy), 0.75)

  state.targetface = { ...state.targetface, happiness, derp: 0, px, py }
}

function handleMouseLeave() {
  state.targetface = { ...config.states.normal.face }
}

onMounted(() => {
  updateFace()
})

onUnmounted(() => {
  if (state.animationId) {
    cancelAnimationFrame(state.animationId)
    state.animationId = null
  }
})

</script>

<template>
  <section class="container" ref="containerRef" @mousemove="handleMouseMove" @mouseleave="handleMouseLeave">
    <div class="content">
      <h1 class="title">{{ state.ui.titleText }}</h1>
      <h2 class="subtitle">{{ state.ui.subtitleText }}</h2>
      <figure class="face-slider" :style="{
        '--happiness': state.currentface.happiness,
        '--derp': state.currentface.derp,
        '--px': state.currentface.px,
        '--py': state.currentface.py
      }">
        <div class="face-slider-blush face-slider-blush-left"></div>
        <div class="face-slider-blush face-slider-blush-right"></div>
        <div class="face-slider-eye face-slider-eye-left"></div>
        <div class="face-slider-eye face-slider-eye-right"></div>
        <div class="face-slider-mouth"></div>
      </figure>
      <button class="button button-happy" ref="btnHappyRef" @click="happy" :style="{
        visibility: state.uiStyle.btnHappyVisibility,
        transform: state.uiStyle.btnHappyTransform
      }">
        {{ state.ui.btnHappyText }}
      </button>
      <button class="button button-unhappy" ref="btnUnhappyRef" @click="unHappy" :style="{
        visibility: state.uiStyle.btnUnhappyVisibility,
        position: state.uiStyle.btnUnhappyPosition,
        left: state.uiStyle.btnUnhappyLeft,
        top: state.uiStyle.btnUnhappyTop
      }">
        {{ state.ui.btnUnhappyText }}
      </button>
    </div>
  </section>
</template>

<style scoped>
.container {
  display: flex;
  flex-direction: column;
  overflow: hidden;
  width: 72em;
  max-width: 100%;
  min-width: 34em;
  height: 50em;
  max-height: 100%;
  min-height: 46em;
  background-color: #ffffff;
  border-radius: 1em;
  border: 1px solid #e0e0e0;
  box-shadow: 0 8px 24px rgba(0, 0, 0, 0.1);
  font-size: 10px;
}

@media screen and (max-width: 480px) {
  .container {
    font-size: 5px;
  }
}

.container,
.container * {
  box-sizing: border-box;
}

.content {
  flex: 1;
  display: flex;
  align-items: flex-end;
  justify-content: space-between;
  position: relative;
  margin: 2em 4em;
  font-size: 100%;
}

.title {
  margin: 0;
  padding: 0;
  position: absolute;
  top: 5%;
  text-align: center;
  width: 100%;
  font-size: 2em;
}

.subtitle {
  margin: 0;
  padding: 0;
  position: absolute;
  top: 15%;
  text-align: center;
  width: 100%;
  font-size: 1.5em;
}

.button {
  color: #fff;
  border-radius: 1em;
  text-decoration: none;
  padding: 1em 2em;
  margin-bottom: 1em;
  text-align: center;
  font-size: 1.5em;
  transition: background-color 0.3s, transform 0.3s;
}

.button-happy {
  background-color: #6a4;
  border: none;
}

.button-happy:hover {
  background-color: #81c061;
}

.button-unhappy {
  background-color: #a43;
  border: none;
}

.button-unhappy:hover {
  background-color: #c85a48;
}

.face-slider {
  --happiness: 0.9;
  --derp: 1;
  --px: 0.5;
  --py: 0.5;
  width: 22em;
  max-width: 100%;
  height: 22em;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  background-image: radial-gradient(#f7e0b2, #eb5);
  border-radius: 100%;
  overflow: hidden;
  margin: 0;
  align-self: center;
  flex: 0 0 auto;
  border: solid 2px #ecb23e;
  box-shadow: 0px 0px 5px rgba(0, 0, 0, 0.2);
}

.face-slider,
.face-slider * {
  position: absolute;
}

.face-slider::before {
  content: '';
  display: block;
  width: 100%;
  height: 100%;
  top: 0;
  left: 0;
  background-image: linear-gradient(to bottom, #5a8, rgba(85, 170, 136, 0));
  opacity: calc(1 - var(--happiness));
}

.face-slider-blush {
  width: 20%;
  height: 10%;
  background-color: rgba(255, 100, 100, 0.3);
  border: 3px solid rgba(255, 100, 100, 0.3);
  top: calc(45% + var(--py) * 10%);
  border-radius: 100%;
  opacity: calc(var(--happiness) * var(--happiness) * 0.9 + 0.1);
}

.face-slider-blush-left {
  left: calc(7% + var(--px) * 2%);
}

.face-slider-blush-right {
  right: calc(9% - var(--px) * 2%);
}

.face-slider-eye {
  width: calc(26% - var(--happiness) * 2%);
  height: calc(26% - var(--happiness) * 2%);
  background-color: #f6f6f6;
  border-radius: 100%;
  top: calc(25% + var(--py) * 10%);
  overflow: hidden;
}

.face-slider-eye-left {
  left: calc(18% + var(--px) * 4%);
}

.face-slider-eye-left::after {
  transform: translate(calc((var(--px) + var(--derp) * 0.5) * 100%), calc((var(--py) + var(--derp) * 0.5) * 100%));
}

.face-slider-eye-right {
  right: calc(22% - var(--px) * 4%);
}

.face-slider-eye-right::after {
  transform: translate(calc((var(--px) + var(--derp) * -0.3) * 100%), calc((var(--py) + var(--derp) * -0.3) * 100%));
}

.face-slider-eye::after {
  content: '';
  display: block;
  background-color: #421;
  width: calc(55% - var(--happiness) * 10%);
  height: calc(55% - var(--happiness) * 10%);
  border-radius: 100%;
}

.face-slider-mouth {
  width: calc(51% - var(--happiness) * 2%);
  height: calc(26% - var(--happiness) * 2%);
  background-color: #a33;
  border-radius: calc((1 - var(--happiness)) * 10em) calc((1 - var(--happiness)) * 10em) calc(var(--happiness) * 16em) calc(var(--happiness) * 16em);
  top: calc(57.5% + var(--py) * 5%);
  left: calc(47.5% + var(--px) * 5%);
  transform: translateX(-50%);
  overflow: hidden;
  border: 3px solid #962d2d;
}

.face-slider-mouth::before {
  content: '';
  display: block;
  position: absolute;
  width: 20%;
  height: 20%;
  top: 0;
  left: 50%;
  background-color: white;
  border-radius: 0 0 0.5em 0.5em;
}

.face-slider-mouth::after {
  content: '';
  display: block;
  position: absolute;
  width: 60%;
  height: 50%;
  left: 10%;
  bottom: 0;
  background-color: rgba(0, 0, 0, 0.2);
  border-radius: 20em 20em 0 0;
}
</style>
