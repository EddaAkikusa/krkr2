*start|�e�X�g�V�i���I

[clearlayers]
[seladd text="KAGEX �g�����C��"            target=*layer]
[seladd text="���[���h�g��:�L�����N�^"     target=*char]
[seladd text="���[���h�g��:���C��"         target=*envlayer]
[seladd text="���[���h�g��:�g�����W�V����" target=*transition]
[seladd text="���[���h�g��:�g���A�N�V����" target=*action]
[select]

*layer|
[call storage=layer.ks]
[jump target=*start]

*char|
[scenestart storage=char.sc]
[sceneplay]
[jump target=*start]

*envlayer|
[scenestart storage=envlayer.sc]
[sceneplay]
[jump target=*start]

*transition|
[scenestart storage=transition.sc]
[sceneplay]
[jump target=*start]

*action|
[scenestart storage=action.sc]
[sceneplay]
[jump target=*start]
