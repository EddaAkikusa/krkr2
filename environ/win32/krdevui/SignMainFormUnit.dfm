object SignMainForm: TSignMainForm
  Left = 78
  Top = 12
  ActiveControl = PageControl
  BorderStyle = bsDialog
  Caption = '�L�[�E�����c�[��'
  ClientHeight = 305
  ClientWidth = 369
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '�l�r �o�S�V�b�N'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object CloseButton: TButton
    Left = 147
    Top = 272
    Width = 75
    Height = 25
    Cancel = True
    Caption = '����(&C)'
    ModalResult = 1
    TabOrder = 0
    OnClick = CloseButtonClick
  end
  object PageControl: TPageControl
    Left = 8
    Top = 8
    Width = 353
    Height = 257
    ActivePage = KeyGenTabSheet
    TabOrder = 1
    object KeyGenTabSheet: TTabSheet
      Caption = '�L�[����'
      object Label1: TLabel
        Left = 8
        Top = 8
        Width = 207
        Height = 12
        Caption = '�������s�����߂̃L�[(��)���쐬���܂��B'
      end
      object Label2: TLabel
        Left = 8
        Top = 24
        Width = 324
        Height = 12
        Caption = '���́u��������v�{�^���������Ɣ閧���ƌ��J������������܂��B'
      end
      object Label3: TLabel
        Left = 8
        Top = 40
        Width = 289
        Height = 12
        Caption = '�����̓y�A�̌��ł��B�����Ƃ���؂ɕۊǂ��Ă��������B'
      end
      object Label4: TLabel
        Left = 12
        Top = 72
        Width = 321
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '�閧���̕��͌��J���Ă͂����܂���'
        Color = clRed
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = '�l�r �o�S�V�b�N'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Layout = tlCenter
      end
      object Label5: TLabel
        Left = 8
        Top = 56
        Width = 179
        Height = 12
        Caption = '���J���͕K�v�ɉ����Č��J���܂��B'
      end
      object Label6: TLabel
        Left = 8
        Top = 149
        Width = 98
        Height = 12
        Caption = '�������ꂽ���J�� :'
      end
      object Label7: TLabel
        Left = 8
        Top = 180
        Width = 98
        Height = 12
        Caption = '�������ꂽ�閧�� :'
      end
      object Label8: TLabel
        Left = 166
        Top = 128
        Width = 12
        Height = 12
        Caption = '��'
      end
      object Label9: TLabel
        Left = 9
        Top = 208
        Width = 327
        Height = 12
        Caption = '�� �E�B���h�E�����Ƃ��ɃN���b�v�{�[�h�̓��e�̓N���A����܂�'
      end
      object GenerateButton: TButton
        Left = 135
        Top = 96
        Width = 75
        Height = 25
        Caption = '��������(&G)'
        Default = True
        TabOrder = 0
        OnClick = GenerateButtonClick
      end
      object GeneratedPublicKeyEdit: TEdit
        Left = 112
        Top = 145
        Width = 145
        Height = 20
        ReadOnly = True
        TabOrder = 1
      end
      object GeneratedPrivateKeyEdit: TEdit
        Left = 112
        Top = 176
        Width = 145
        Height = 20
        Color = clWhite
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clRed
        Font.Height = -12
        Font.Name = '�l�r �o�S�V�b�N'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        TabOrder = 2
      end
      object CopyPublicKeyButton: TButton
        Left = 264
        Top = 143
        Width = 75
        Height = 25
        Hint = '�N���b�v�{�[�h�Ɍ��J�����R�s�[���܂�'
        Caption = '�R�s�[(&B)'
        Enabled = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        OnClick = CopyPublicKeyButtonClick
      end
      object CopyPrivateKeyButton: TButton
        Left = 264
        Top = 174
        Width = 75
        Height = 25
        Hint = '�N���b�v�{�[�h�ɔ閧�����R�s�[���܂�'
        Caption = '�R�s�[(&P)'
        Enabled = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        OnClick = CopyPrivateKeyButtonClick
      end
    end
    object SignTabSheet: TTabSheet
      Caption = '����'
      ImageIndex = 1
      object Label10: TLabel
        Left = 8
        Top = 8
        Width = 210
        Height = 12
        Caption = '���������܂��B�閧�����w�肵�Ă��������B'
      end
      object Label11: TLabel
        Left = 16
        Top = 37
        Width = 57
        Height = 12
        Caption = '�閧��(&P) :'
        FocusControl = SignPrivateKeyEdit
      end
      object Label12: TLabel
        Left = 8
        Top = 72
        Width = 249
        Height = 12
        Caption = '�������s���ΏۂƂȂ�t�@�C�����w�肵�Ă��������B'
      end
      object Label13: TLabel
        Left = 8
        Top = 136
        Width = 274
        Height = 12
        Caption = '���o�[�W�����ł͋g���g���{�̂ɂ̂ݏ����\�ł��B'
        Visible = False
      end
      object Label14: TLabel
        Left = 16
        Top = 100
        Width = 86
        Height = 12
        Caption = '�Ώۃt�@�C��(&T) :'
        FocusControl = SignFileNameEdit
      end
      object Label15: TLabel
        Left = 8
        Top = 152
        Width = 232
        Height = 12
        Caption = '�t�@�C�������̃E�B���h�E�Ƀh���b�v���邱�Ƃł�'
      end
      object Label16: TLabel
        Left = 8
        Top = 168
        Width = 207
        Height = 12
        Caption = '�Ώۃt�@�C�����w�肷�邱�Ƃ��o���܂��B'
      end
      object SignDoneLabel: TLabel
        Left = 56
        Top = 216
        Width = 96
        Height = 12
        Caption = '�������s���܂����B'
        Visible = False
      end
      object SignPrivateKeyEdit: TEdit
        Left = 88
        Top = 33
        Width = 241
        Height = 20
        Color = clWhite
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clRed
        Font.Height = -12
        Font.Name = '�l�r �o�S�V�b�N'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnChange = SignPrivateKeyEditChange
      end
      object SignFileNameEdit: TEdit
        Left = 112
        Top = 96
        Width = 217
        Height = 20
        TabOrder = 1
        OnChange = SignPrivateKeyEditChange
      end
      object SignFileNameRefButton: TButton
        Left = 264
        Top = 126
        Width = 67
        Height = 25
        Caption = '�Q��(&R) ...'
        TabOrder = 2
        OnClick = SignFileNameRefButtonClick
      end
      object SignButton: TButton
        Left = 132
        Top = 192
        Width = 82
        Height = 25
        Caption = '��������(&S)'
        Default = True
        Enabled = False
        TabOrder = 3
        OnClick = SignButtonClick
      end
    end
    object CheckSignTabSheet: TTabSheet
      Caption = '�����m�F'
      ImageIndex = 2
      object SignatureIsValidLabel: TLabel
        Left = 32
        Top = 216
        Width = 124
        Height = 12
        Caption = '�����͐����Ȃ��̂ł��B'
        Visible = False
      end
      object Label18: TLabel
        Left = 8
        Top = 168
        Width = 207
        Height = 12
        Caption = '�Ώۃt�@�C�����w�肷�邱�Ƃ��o���܂��B'
      end
      object Label19: TLabel
        Left = 8
        Top = 152
        Width = 232
        Height = 12
        Caption = '�t�@�C�������̃E�B���h�E�Ƀh���b�v���邱�Ƃł�'
      end
      object Label20: TLabel
        Left = 16
        Top = 100
        Width = 86
        Height = 12
        Caption = '�Ώۃt�@�C��(&T) :'
        FocusControl = CheckSignFileNameEdit
      end
      object Label21: TLabel
        Left = 8
        Top = 136
        Width = 274
        Height = 12
        Caption = '���o�[�W�����ł͋g���g���{�̂ɂ̂ݏ����\�ł��B'
        Visible = False
      end
      object Label22: TLabel
        Left = 8
        Top = 72
        Width = 290
        Height = 12
        Caption = '�������`�F�b�N����ΏۂƂȂ�t�@�C�����w�肵�Ă��������B'
      end
      object Label23: TLabel
        Left = 16
        Top = 37
        Width = 57
        Height = 12
        Caption = '���J��(&P) :'
        FocusControl = CheckSignPublicKeyEdit
      end
      object Label24: TLabel
        Left = 8
        Top = 8
        Width = 234
        Height = 12
        Caption = '�������m�F���܂��B���J�����w�肵�Ă��������B'
      end
      object SignatureIsInvalidLabel: TLabel
        Left = 184
        Top = 216
        Width = 169
        Height = 12
        Caption = '�����͐����ȕ��ł͂���܂���B'
        Visible = False
      end
      object CheckSignatureButton: TButton
        Left = 132
        Top = 192
        Width = 82
        Height = 25
        Caption = '�`�F�b�N(&S)'
        Default = True
        Enabled = False
        TabOrder = 3
        OnClick = CheckSignatureButtonClick
      end
      object CheckSignFileNameRefButton: TButton
        Left = 264
        Top = 126
        Width = 67
        Height = 25
        Caption = '�Q��(&R) ...'
        TabOrder = 2
        OnClick = CheckSignFileNameRefButtonClick
      end
      object CheckSignFileNameEdit: TEdit
        Left = 112
        Top = 96
        Width = 217
        Height = 20
        TabOrder = 1
        OnChange = CheckSignPublicKeyEditChange
      end
      object CheckSignPublicKeyEdit: TEdit
        Left = 88
        Top = 33
        Width = 241
        Height = 20
        Font.Charset = SHIFTJIS_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = '�l�r �o�S�V�b�N'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnChange = CheckSignPublicKeyEditChange
      end
    end
  end
  object OpenDialog: TOpenDialog
    Left = 20
    Top = 223
  end
end
