object ConfSettingsForm: TConfSettingsForm
  Left = 11
  Top = 15
  Width = 607
  Height = 389
  Caption = '�ݒ�'
  Color = clBtnFace
  Constraints.MinHeight = 389
  Constraints.MinWidth = 607
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '�l�r �o�S�V�b�N'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  PixelsPerInch = 96
  TextHeight = 12
  object SpecifiedFileIsNotKrKrLabel: TLabel
    Left = 8
    Top = 328
    Width = 324
    Height = 12
    Caption = '�w�肳�ꂽ�t�@�C���͋g���g�����s�\�t�@�C���ł͂���܂���'
    Visible = False
  end
  object IconChangeWarnLabel: TLabel
    Left = 8
    Top = 320
    Width = 407
    Height = 12
    Anchors = [akLeft, akBottom]
    Caption = 
      '�� ��x�A�C�R���̕ύX���s���ƕʂ̃A�C�R���ɕύX�ł��Ȃ��Ȃ�\' +
      '��������܂�'
  end
  object FileWroteLabel: TLabel
    Left = 8
    Top = 336
    Width = 98
    Height = 12
    Caption = '�ݒ��ۑ����܂���'
    Visible = False
  end
  object ProgramMayRunningLabel: TLabel
    Left = 8
    Top = 344
    Width = 322
    Height = 12
    Caption = '�t�@�C�����J���܂���(�v���O���������s���̉\��������܂�)'
    Visible = False
  end
  object OKButton: TButton
    Left = 425
    Top = 327
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 0
    OnClick = OKButtonClick
  end
  object CancelButton: TButton
    Left = 505
    Top = 327
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = '�L�����Z��'
    ModalResult = 2
    TabOrder = 1
  end
  inline ConfMainFrame: TConfMainFrame
    Left = 8
    Top = 8
    Width = 578
    Height = 306
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 2
    inherited IconGroupBox: TGroupBox
      Top = 243
      Width = 578
    end
    inherited OptionsGroupBox: TGroupBox
      Width = 578
      Height = 235
      inherited Label2: TLabel
        Left = 360
      end
      inherited OptionsTreeView: TTreeView
        Width = 340
      end
      inherited OptionValueComboBox: TComboBox
        Left = 360
      end
      inherited OptionDescMemo: TMemo
        Left = 360
      end
      inherited RestoreDefaultButton: TButton
        Width = 121
      end
    end
    inherited Panel1: TPanel
      Top = 235
      Width = 578
    end
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'exe'
    Filter = '���s�\�t�@�C��(*.exe)|*.exe|���ׂẴt�@�C��(*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Title = '�g���g�� ���s�\�t�@�C���̑I��'
    Left = 384
    Top = 328
  end
end
