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
[initscene storage=char.sc]
[scenemain]
[jump target=*start]

*envlayer|
[initscene storage=envlayer.sc]
[scenemain]
[jump target=*start]

*transition|
[initscene storage=transition.sc]
[scenemain]
[jump target=*start]

*action|
[initscene storage=action.sc]
[scenemain]
[jump target=*start]
