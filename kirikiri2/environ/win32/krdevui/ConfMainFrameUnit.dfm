object ConfMainFrame: TConfMainFrame
  Left = 0
  Top = 0
  Width = 585
  Height = 303
  TabOrder = 0
  object IconGroupBox: TGroupBox
    Left = 0
    Top = 240
    Width = 585
    Height = 63
    Align = alBottom
    Caption = '�A�C�R��(&I)'
    TabOrder = 0
    object ChangeIconCheck: TCheckBox
      Left = 8
      Top = 32
      Width = 121
      Height = 17
      Caption = '�A�C�R����ύX����'
      TabOrder = 0
    end
    object RefIconButton: TButton
      Left = 168
      Top = 29
      Width = 75
      Height = 20
      Caption = '�Q�� ...'
      TabOrder = 1
      OnClick = RefIconButtonClick
    end
    object IconPanel: TPanel
      Left = 272
      Top = 16
      Width = 41
      Height = 41
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 2
      object IconImage: TImage
        Left = 2
        Top = 2
        Width = 37
        Height = 37
        Align = alClient
        Center = True
        Transparent = True
      end
    end
  end
  object OptionsGroupBox: TGroupBox
    Left = 0
    Top = 0
    Width = 585
    Height = 232
    Align = alClient
    Caption = '�I�v�V����(&O)'
    Enabled = False
    TabOrder = 1
    object Label1: TLabel
      Left = 8
      Top = 16
      Width = 109
      Height = 12
      Caption = '�I�v�V�����̖��O(&N) :'
      FocusControl = OptionsTreeView
    end
    object Label2: TLabel
      Left = 363
      Top = 16
      Width = 97
      Height = 12
      Anchors = [akTop, akRight]
      Caption = '�I�v�V�����̒l(&V) :'
    end
    object OptionsReadFailedLabel: TLabel
      Left = 120
      Top = 16
      Width = 210
      Height = 12
      Caption = '�I�v�V�������̓ǂݍ��݂Ɏ��s���܂���'
      Visible = False
    end
    object ReadingOptionsLabel: TLabel
      Left = 232
      Top = 16
      Width = 155
      Height = 12
      Caption = '�I�v�V��������ǂݍ��ݒ� ...'
      Visible = False
    end
    object Label3: TLabel
      Left = 360
      Top = 88
      Width = 93
      Height = 12
      Anchors = [akTop, akRight]
      Caption = '�I�v�V�����̐��� :'
    end
    object OptionsTreeView: TTreeView
      Left = 8
      Top = 32
      Width = 348
      Height = 185
      Anchors = [akLeft, akTop, akRight, akBottom]
      HideSelection = False
      Indent = 19
      ReadOnly = True
      RowSelect = True
      ShowRoot = False
      TabOrder = 0
      OnChange = OptionsTreeViewChange
    end
    object OptionValueComboBox: TComboBox
      Left = 363
      Top = 32
      Width = 209
      Height = 20
      Style = csDropDownList
      Anchors = [akTop, akRight]
      Enabled = False
      ItemHeight = 12
      TabOrder = 1
      OnChange = OptionValueComboBoxChange
    end
    object OptionDescMemo: TMemo
      Left = 363
      Top = 104
      Width = 209
      Height = 113
      Anchors = [akTop, akRight, akBottom]
      BorderStyle = bsNone
      Color = clBtnFace
      ReadOnly = True
      ScrollBars = ssVertical
      TabOrder = 2
    end
    object RestoreDefaultButton: TButton
      Left = 363
      Top = 56
      Width = 118
      Height = 20
      Anchors = [akTop, akRight]
      Caption = '�f�t�H���g�ɖ߂�(&D)'
      Enabled = False
      TabOrder = 3
      OnClick = RestoreDefaultButtonClick
    end
    object InvisibleCheckBox: TCheckBox
      Left = 508
      Top = 56
      Width = 65
      Height = 17
      Anchors = [akTop, akRight]
      Caption = '��\��'
      Enabled = False
      TabOrder = 4
      OnClick = InvisibleCheckBoxClick
    end
    object OptionValueEdit: TEdit
      Left = 360
      Top = 72
      Width = 209
      Height = 20
      TabOrder = 5
      Visible = False
      OnChange = OptionValueEditChange
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 232
    Width = 585
    Height = 8
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
  end
  object OpenPictureDialog: TOpenPictureDialog
    Filter = '�A�C�R�� (*.ico)|*.ico'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = '�A�C�R���̎w��'
    Left = 136
    Top = 248
  end
end
