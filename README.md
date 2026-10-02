# Tarrou-Renderer

<div align="center">
  <img src="https://github.com/BOLTB0X/Metal-API/blob/main/img/Tarrou/%EB%A9%94%EC%89%AC%EB%A0%9B03_PCF02.png?raw=true" width="500" style="border:1px solid #ddd; border-radius:4px;" />
  <br>
  <p><strong>Apple Metal 기반의 IncetenceMeshlet 렌더러</strong></p>
</div>

## Stack

- C++17 / MSL 3.0+
- Apple Metal / MetalKit / ModelIO
- meshoptimizer / ImGui
- Xcode 27.0
- Apple Silicon / macOS Tahoe

## Feature

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/Metal-API/blob/main/img/Tarrou/%EB%A9%94%EC%89%AC%EB%A0%9B01.png?raw=true" 
             alt="image 1" 
             style="width:70%; height:70%; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      Meshlet : 메시를 작은 클러스터 단위로 분할
      </p>
      </td>
    </tr>
  </table>
</p>

<p align="center">
  <table style="width:100% text-align:center; border-spacing:20px;">
    <tr>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/Metal-API/blob/main/img/Tarrou/%EB%A9%94%EC%89%AC%EB%A0%9B04_%EB%85%B8%EB%A7%90%EC%BD%98%EC%BB%AC%EB%A7%8102.png?raw=true" 
             alt="normal culling" 
             style="width:70%; height:70%; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        <img src="https://github.com/BOLTB0X/Metal-API/blob/main/img/Tarrou/%EB%A9%94%EC%89%AC%EB%A0%9B04_%EB%85%B8%EB%A7%90%EC%BD%98%EC%BB%AC%EB%A7%8103.png?raw=true" 
             alt="normal culling" 
             style="width:70%; height:70%; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      Normal Culling : 면의 노말 방향을 기준으로 보이지 않는 Meshlet을 컬링
      </p>
      </td>
    </tr>
  </table>
</p>


## Ref.

- [Github: metal-by-example - MetalMeshletCulling](https://github.com/metal-by-example/MetalMeshletCulling/tree/master)

- [metalbyexample - Mesh Shaders and Meshlet Culling in Metal 3](https://metalbyexample.com/mesh-shaders/)

- [모델 출처 - Happy Buddha (Stanford)](https://sketchfab.com/3d-models/happy-buddha-stanford-5f2a444ff26c4a3bb194f6d79502ee54)
