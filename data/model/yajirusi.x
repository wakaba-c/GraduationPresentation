xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 28;
 -0.00250;19.55647;-201.15350;,
 0.00250;19.55647;-201.15350;,
 0.00250;-12.35300;-201.15350;,
 -0.00250;-12.35300;-201.15350;,
 0.00250;19.55647;-201.15350;,
 100.00000;19.55647;-1.15347;,
 100.00000;-12.35300;-1.15347;,
 0.00250;-12.35300;-201.15350;,
 -100.00000;19.55647;-1.15347;,
 -0.00250;19.55647;-201.15350;,
 -0.00250;-12.35300;-201.15350;,
 -100.00000;-12.35300;-1.15347;,
 0.00250;19.55647;-201.15350;,
 -0.00250;19.55647;-201.15350;,
 -0.00250;-12.35300;-201.15350;,
 0.00250;-12.35300;-201.15350;,
 -22.62089;19.55647;-1.15347;,
 -100.00000;19.55647;-1.15347;,
 -100.00000;-12.35300;-1.15347;,
 -22.62089;-12.35300;-1.15347;,
 -61.91710;-12.35300;233.89639;,
 -61.91710;19.55647;233.89639;,
 56.82213;-12.35300;233.89639;,
 56.82213;19.55647;233.89639;,
 20.87781;-12.35300;-1.15347;,
 100.00000;-12.35300;-1.15347;,
 100.00000;19.55647;-1.15347;,
 20.87781;19.55647;-1.15347;;
 
 12;
 4;0,1,2,3;,
 4;4,5,6,7;,
 4;8,9,10,11;,
 4;8,5,12,13;,
 4;14,15,6,11;,
 4;16,17,18,19;,
 4;20,21,16,19;,
 4;22,23,21,20;,
 4;24,25,26,27;,
 4;24,27,23,22;,
 4;16,21,23,27;,
 4;19,24,22,20;;
 
 MeshMaterialList {
  1;
  12;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.800000;0.000000;0.069020;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  9;
  0.000000;0.000000;-1.000000;,
  0.894432;0.000000;-0.447205;,
  -0.894432;0.000000;-0.447205;,
  0.000000;0.000000;1.000000;,
  -0.986311;0.000000;-0.164894;,
  0.000000;1.000000;0.000000;,
  0.000000;-1.000000;-0.000000;,
  0.988509;0.000000;-0.151165;,
  0.000000;-1.000000;-0.000000;;
  12;
  4;0,0,0,0;,
  4;1,1,1,1;,
  4;2,2,2,2;,
  4;5,5,5,5;,
  4;6,6,6,6;,
  4;3,3,3,3;,
  4;4,4,4,4;,
  4;3,3,3,3;,
  4;3,3,3,3;,
  4;7,7,7,7;,
  4;5,5,5,5;,
  4;8,8,8,8;;
 }
 MeshTextureCoords {
  28;
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.639116;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.639116;1.000000;,
  0.626104;1.000000;,
  0.626104;0.000000;,
  0.399371;1.000000;,
  0.399371;0.000000;,
  0.369599;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.369599;0.000000;;
 }
}
