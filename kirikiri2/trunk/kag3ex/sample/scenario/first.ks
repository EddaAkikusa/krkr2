@call storage="world.ks"
@loadplugin module=wuvorbis.dll
@loadplugin module=extrans.dll

*start|�X�^�[�g

[clearlayers]

[seladd text="���C���e�X�g" storage=layer.ks target=*start]
[seladd text="�L�����N�^�e�X�g" storage=char.ks target=*start]
[seladd text="�g�����W�V�����e�X�g" storage=transition.ks target=*start]
[seladd text="�����C���e�X�g" storage=envlayer.ks target=*start]
[seladd text="�I�����ϐ��e�X�g" exp='f.test=100']
[select]

[layopt layer=message0 visible]
�ݒ肳�ꂽ�l:[emb exp="f.test"][l]

[jump target=*start]
