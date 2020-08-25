object frmLips: TfrmLips
  Left = 0
  Top = 0
  Caption = 'LIPS - Lua Image Processing System'
  ClientHeight = 674
  ClientWidth = 1062
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  WindowState = wsMaximized
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object panMain: TPanel
    Left = 0
    Top = 0
    Width = 776
    Height = 643
    Align = alClient
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Ctl3D = False
    ParentCtl3D = False
    TabOrder = 0
    object pageMVControl: TcxPageControl
      Left = 0
      Top = 0
      Width = 774
      Height = 641
      Align = alClient
      TabOrder = 0
      Properties.ActivePage = tabNormal
      Properties.CustomButtons.Buttons = <>
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Darkroom'
      ClientRectBottom = 636
      ClientRectLeft = 5
      ClientRectRight = 769
      ClientRectTop = 29
      object tabNormal: TcxTabSheet
        Caption = 'Single image view'
        ImageIndex = 0
        object panSingle: TPanel
          Left = 0
          Top = 0
          Width = 185
          Height = 607
          Align = alLeft
          BevelOuter = bvNone
          TabOrder = 0
          object mView: TImageEnMView
            Left = 0
            Top = 0
            Width = 180
            Height = 536
            Background = clBtnFace
            Ctl3D = False
            ParentCtl3D = False
            StoreType = ietThumb
            ThumbWidth = 150
            ThumbHeight = 150
            HorizBorder = 4
            VertBorder = 4
            ThumbnailOptionsEx = [ietxCenterThumbnailColumn, ietxShowIconForUnknownFormat, ietxShowIconWhileLoading, ietxEnableInternalIcons, ietxStretchSmallImages]
            OnImageSelect = mViewImageSelect
            GridWidth = 1
            SelectionColor = 13542013
            ImageCacheSize = 10
            Style = iemsFlat
            AutoAdjustStyle = False
            ThumbnailsBackground = clBtnFace
            ThumbnailsBackgroundSelected = clBtnFace
            MultiSelectionOptions = []
            ThumbnailsBorderWidth = 0
            DefaultTopText = iedtNone
            DefaultInfoText = iedtNone
            DefaultBottomText = iedtNone
            Align = alLeft
            ThumbnailResampleFilter = rfFastLinear
            ThumbnailDisplayFilter = rfFastLinear
            SelectedFontColor = clNone
            MouseWheelParams.Action = iemwVScroll
            MouseWheelParamsAlt.Action = iemwZoom
            TabOrder = 0
          end
          object panMVaction: TPanel
            Left = 0
            Top = 536
            Width = 185
            Height = 71
            Align = alBottom
            BevelOuter = bvNone
            TabOrder = 1
            object btnClearAdditional: TSpeedButton
              Left = 5
              Top = 11
              Width = 139
              Height = 22
              Caption = 'Clear additional images'
              OnClick = btnClearAdditionalClick
            end
            object btnCopy: TSpeedButton
              Left = 5
              Top = 39
              Width = 146
              Height = 22
              Caption = 'Copy or swap image(s)'
              PopupMenu = popupCopy
              OnClick = btnCopyClick
            end
          end
        end
        object ieViewSingle: TImageEnVect
          Left = 185
          Top = 0
          Width = 579
          Height = 607
          Background = clBtnFace
          Ctl3D = False
          ParentCtl3D = False
          LegacyBitmap = False
          SelectionBase = iesbBitmap
          MouseWheelParams.Action = iemwZoom
          Align = alClient
          TabOrder = 1
        end
      end
      object tabLayers: TcxTabSheet
        Caption = 'Layers view'
        ImageIndex = 1
        object panLayers: TPanel
          Left = 0
          Top = 0
          Width = 185
          Height = 607
          Align = alLeft
          BevelOuter = bvNone
          TabOrder = 0
          object layersMView: TImageEnLayerMView
            Left = 0
            Top = 0
            Width = 181
            Height = 446
            Background = clBtnFace
            Ctl3D = False
            ParentCtl3D = False
            StoreType = ietThumb
            ThumbWidth = 150
            ThumbHeight = 150
            HorizBorder = 8
            VertBorder = 8
            BottomGap = 6
            UpperGap = 6
            LeftGap = 6
            TextMargin = 4
            ThumbnailOptionsEx = [ietxCenterThumbnailColumn, ietxShowIconForUnknownFormat, ietxShowIconWhileLoading, ietxEnableInternalIcons, ietxStretchSmallImages]
            OnImageSelect = layersMViewImageSelect
            KeyInteract = []
            GridWidth = 1
            SelectionWidth = 1
            SelectionColor = 13542013
            ImageCacheSize = 100
            Style = iemsFlat
            AutoAdjustStyle = True
            ThumbnailsBackground = clWindow
            ThumbnailsBackgroundSelected = 16575194
            EnableMultiSelect = True
            MultiSelectionOptions = [iemoRegion, iemoOptimizeForDragging, iemoSelectOnRightClick, iemoHideSelectionRect]
            ThumbnailsBorderWidth = 1
            DefaultTopText = iedtLayerName
            DefaultInfoText = iedtNone
            DefaultBottomText = iedtLayerPosition
            Align = alLeft
            ThumbnailResampleFilter = rfFastLinear
            ThumbnailDisplayFilter = rfFastLinear
            SelectedFontColor = clNone
            MouseWheelParams.Action = iemwVScroll
            MouseWheelParamsAlt.Action = iemwZoom
            ParentShowHint = False
            ShowHint = True
            TabOrder = 0
            LayerOrder = ieloBackgroundFirst
          end
          object panBlender: TPanel
            Left = 0
            Top = 446
            Width = 185
            Height = 161
            Align = alBottom
            BevelOuter = bvNone
            TabOrder = 1
            object Label4: TLabel
              Left = 50
              Top = 5
              Width = 69
              Height = 13
              Caption = 'Blending mode'
            end
            object Label5: TLabel
              Left = 50
              Top = 56
              Width = 66
              Height = 13
              Caption = 'Transparency'
            end
            object btnLayersMerge: TSpeedButton
              Left = 4
              Top = 130
              Width = 169
              Height = 27
              Caption = 'Merge layers to new image'
              OnClick = btnLayersMergeClick
            end
            object chkLayersSB: TpxCheckLabel
              Left = 7
              Top = 103
              Width = 237
              Height = 22
              CheckBoxSize = 14
              SmoothText = False
              PenWidth = 1
              PenCurvature = 2
              ColorBrush = 7131391
              CaptionX = 20
              CaptionY = 4
              Checked = True
              Caption = 'Same blending for all layers'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWhite
              Font.Height = -11
              Font.Name = 'Tahoma'
              Font.Style = []
              OnValueChange = chkLayersSBValueChange
            end
            object editBlend: TpxSpinSlider
              Left = 7
              Top = 76
              Width = 162
              Height = 21
              LayoutStyle = lyPixopedia
              SliderThickness = 8
              SliderLength = 100
              MaxInt = 255
              MaxFloat = 100.000000000000000000
              FloatIncrementSmall = 0.200000000000000000
              FloatIncrementLarge = 10.000000000000000000
              FloatDecimals = 4
              IntValue = 255
              IntIncrementSmall = 1
              IntIncrementLarge = 10
              CheckLabel.Width = 1
              CheckLabel.Height = 21
              CheckLabel.CheckBoxSize = 10
              CheckLabel.CheckBoxVisible = False
              CheckLabel.SmoothText = False
              CheckLabel.PenWidth = 1
              CheckLabel.CaptionX = 3
              CheckLabel.CaptionY = 3
              CheckLabel.Shape = chkSemiLeft
              CheckLabel.Caption = 'editMeshThickness'
              PenWidth = 1
              ColorEdit = 2105376
              Color = clBtnFace
              ParentColor = False
              ParentBackground = True
              OnValueChange = editBlendValueChange
            end
            object comboBlend: TcxComboBox
              Left = 2
              Top = 25
              Properties.DropDownListStyle = lsFixedList
              Properties.OnChange = comboBlendPropertiesChange
              Style.BorderStyle = ebsOffice11
              Style.LookAndFeel.NativeStyle = False
              Style.LookAndFeel.SkinName = 'Darkroom'
              Style.Shadow = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.SkinName = 'Darkroom'
              StyleHot.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.SkinName = 'Darkroom'
              TabOrder = 1
              Width = 172
            end
          end
        end
        object ieViewLayers: TImageEnVect
          Left = 185
          Top = 0
          Width = 579
          Height = 607
          Background = clBtnFace
          Ctl3D = False
          ParentCtl3D = False
          LegacyBitmap = False
          MouseInteract = [miMoveLayers]
          MouseWheelParams.Action = iemwZoom
          Align = alClient
          TabOrder = 1
        end
      end
    end
  end
  object pageControl: TcxPageControl
    Left = 776
    Top = 0
    Width = 286
    Height = 643
    Align = alRight
    TabOrder = 1
    Properties.ActivePage = cxTabSheet1
    Properties.CustomButtons.Buttons = <>
    LookAndFeel.NativeStyle = False
    LookAndFeel.SkinName = 'Darkroom'
    ClientRectBottom = 638
    ClientRectLeft = 5
    ClientRectRight = 281
    ClientRectTop = 29
    object cxTabSheet1: TcxTabSheet
      Caption = 'Lua script'
      ImageIndex = 0
      object panCtrls: TPanel
        Left = 0
        Top = 0
        Width = 276
        Height = 609
        Align = alClient
        BevelOuter = bvNone
        BorderStyle = bsSingle
        Ctl3D = False
        ParentCtl3D = False
        TabOrder = 0
        object panTop: TPanel
          Left = 0
          Top = 0
          Width = 274
          Height = 273
          Align = alTop
          BevelOuter = bvNone
          BorderStyle = bsSingle
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          object listScripts: TAdvSmoothListBox
            Left = 0
            Top = 0
            Width = 272
            Height = 271
            Cursor = crDefault
            Fill.Color = clNone
            Fill.ColorTo = clNone
            Fill.ColorMirror = clNone
            Fill.ColorMirrorTo = clNone
            Fill.GradientType = gtSolid
            Fill.GradientMirrorType = gtSolid
            Fill.BorderColor = clBlack
            Fill.Rounding = 0
            Fill.ShadowOffset = 0
            Fill.Glow = gmNone
            Items = <>
            ItemAppearance.FillAlternate.Color = 16773091
            ItemAppearance.FillAlternate.ColorTo = 16768452
            ItemAppearance.FillAlternate.ColorMirror = clNone
            ItemAppearance.FillAlternate.ColorMirrorTo = clNone
            ItemAppearance.FillAlternate.GradientType = gtVertical
            ItemAppearance.FillAlternate.GradientMirrorType = gtSolid
            ItemAppearance.FillAlternate.BorderColor = clNone
            ItemAppearance.FillAlternate.Rounding = 0
            ItemAppearance.FillAlternate.ShadowOffset = 0
            ItemAppearance.FillAlternate.Glow = gmNone
            ItemAppearance.Fill.Color = 3355443
            ItemAppearance.Fill.ColorTo = clBlack
            ItemAppearance.Fill.ColorMirror = clBlack
            ItemAppearance.Fill.ColorMirrorTo = 3355443
            ItemAppearance.Fill.GradientType = gtVertical
            ItemAppearance.Fill.GradientMirrorType = gtVertical
            ItemAppearance.Fill.BorderColor = clGray
            ItemAppearance.Fill.Rounding = 0
            ItemAppearance.Fill.RoundingType = rtNone
            ItemAppearance.Fill.ShadowOffset = 0
            ItemAppearance.Fill.Glow = gmNone
            ItemAppearance.FillSelected.Color = 7131391
            ItemAppearance.FillSelected.ColorTo = clNone
            ItemAppearance.FillSelected.ColorMirror = clNone
            ItemAppearance.FillSelected.ColorMirrorTo = clNone
            ItemAppearance.FillSelected.GradientType = gtVertical
            ItemAppearance.FillSelected.GradientMirrorType = gtVertical
            ItemAppearance.FillSelected.BorderColor = 16765357
            ItemAppearance.FillSelected.Rounding = 0
            ItemAppearance.FillSelected.ShadowOffset = 0
            ItemAppearance.FillSelected.Glow = gmNone
            ItemAppearance.FillSelected.GlowGradientColor = 8122111
            ItemAppearance.FillSelectedAlternate.Color = 11196927
            ItemAppearance.FillSelectedAlternate.ColorTo = 7257087
            ItemAppearance.FillSelectedAlternate.ColorMirror = clNone
            ItemAppearance.FillSelectedAlternate.ColorMirrorTo = clNone
            ItemAppearance.FillSelectedAlternate.GradientType = gtVertical
            ItemAppearance.FillSelectedAlternate.GradientMirrorType = gtSolid
            ItemAppearance.FillSelectedAlternate.BorderColor = clNone
            ItemAppearance.FillSelectedAlternate.Rounding = 0
            ItemAppearance.FillSelectedAlternate.ShadowOffset = 0
            ItemAppearance.FillSelectedAlternate.Glow = gmNone
            ItemAppearance.FillDisabled.Color = 15921906
            ItemAppearance.FillDisabled.ColorTo = 11974326
            ItemAppearance.FillDisabled.ColorMirror = 11974326
            ItemAppearance.FillDisabled.ColorMirrorTo = 15921906
            ItemAppearance.FillDisabled.GradientType = gtVertical
            ItemAppearance.FillDisabled.GradientMirrorType = gtVertical
            ItemAppearance.FillDisabled.BorderColor = 16765357
            ItemAppearance.FillDisabled.Rounding = 0
            ItemAppearance.FillDisabled.ShadowOffset = 0
            ItemAppearance.FillDisabled.Glow = gmNone
            ItemAppearance.ProgressAppearance.BackGroundFill.Color = 16765615
            ItemAppearance.ProgressAppearance.BackGroundFill.ColorTo = 16765615
            ItemAppearance.ProgressAppearance.BackGroundFill.ColorMirror = clNone
            ItemAppearance.ProgressAppearance.BackGroundFill.ColorMirrorTo = clNone
            ItemAppearance.ProgressAppearance.BackGroundFill.GradientType = gtVertical
            ItemAppearance.ProgressAppearance.BackGroundFill.GradientMirrorType = gtSolid
            ItemAppearance.ProgressAppearance.BackGroundFill.BorderColor = clSilver
            ItemAppearance.ProgressAppearance.BackGroundFill.Rounding = 0
            ItemAppearance.ProgressAppearance.BackGroundFill.ShadowOffset = 0
            ItemAppearance.ProgressAppearance.BackGroundFill.Glow = gmNone
            ItemAppearance.ProgressAppearance.ProgressFill.Color = 16773091
            ItemAppearance.ProgressAppearance.ProgressFill.ColorTo = 16768452
            ItemAppearance.ProgressAppearance.ProgressFill.ColorMirror = 16765357
            ItemAppearance.ProgressAppearance.ProgressFill.ColorMirrorTo = 16767936
            ItemAppearance.ProgressAppearance.ProgressFill.GradientType = gtVertical
            ItemAppearance.ProgressAppearance.ProgressFill.GradientMirrorType = gtVertical
            ItemAppearance.ProgressAppearance.ProgressFill.BorderColor = 16765357
            ItemAppearance.ProgressAppearance.ProgressFill.Rounding = 0
            ItemAppearance.ProgressAppearance.ProgressFill.ShadowOffset = 0
            ItemAppearance.ProgressAppearance.ProgressFill.Glow = gmNone
            ItemAppearance.ProgressAppearance.Font.Charset = DEFAULT_CHARSET
            ItemAppearance.ProgressAppearance.Font.Color = clWindowText
            ItemAppearance.ProgressAppearance.Font.Height = -11
            ItemAppearance.ProgressAppearance.Font.Name = 'Tahoma'
            ItemAppearance.ProgressAppearance.Font.Style = []
            ItemAppearance.ProgressAppearance.ProgressFont.Charset = DEFAULT_CHARSET
            ItemAppearance.ProgressAppearance.ProgressFont.Color = clWindowText
            ItemAppearance.ProgressAppearance.ProgressFont.Height = -11
            ItemAppearance.ProgressAppearance.ProgressFont.Name = 'Tahoma'
            ItemAppearance.ProgressAppearance.ProgressFont.Style = []
            ItemAppearance.ProgressAppearance.ValueFormat = '%.0f%%'
            ItemAppearance.ButtonAppearance.Font.Charset = DEFAULT_CHARSET
            ItemAppearance.ButtonAppearance.Font.Color = clWindowText
            ItemAppearance.ButtonAppearance.Font.Height = -11
            ItemAppearance.ButtonAppearance.Font.Name = 'Tahoma'
            ItemAppearance.ButtonAppearance.Font.Style = []
            ItemAppearance.InfoFill.ColorMirror = clNone
            ItemAppearance.InfoFill.ColorMirrorTo = clNone
            ItemAppearance.InfoFill.GradientType = gtVertical
            ItemAppearance.InfoFill.GradientMirrorType = gtSolid
            ItemAppearance.InfoFill.BorderColor = clNone
            ItemAppearance.InfoFill.Rounding = 0
            ItemAppearance.InfoFill.ShadowOffset = 0
            ItemAppearance.InfoFill.Glow = gmNone
            ItemAppearance.InfoFillSelected.ColorMirror = clNone
            ItemAppearance.InfoFillSelected.ColorMirrorTo = clNone
            ItemAppearance.InfoFillSelected.GradientType = gtVertical
            ItemAppearance.InfoFillSelected.GradientMirrorType = gtSolid
            ItemAppearance.InfoFillSelected.BorderColor = clNone
            ItemAppearance.InfoFillSelected.Rounding = 0
            ItemAppearance.InfoFillSelected.ShadowOffset = 0
            ItemAppearance.InfoFillSelected.Glow = gmNone
            ItemAppearance.InfoFillDisabled.ColorMirror = clNone
            ItemAppearance.InfoFillDisabled.ColorMirrorTo = clNone
            ItemAppearance.InfoFillDisabled.GradientType = gtVertical
            ItemAppearance.InfoFillDisabled.GradientMirrorType = gtSolid
            ItemAppearance.InfoFillDisabled.BorderColor = clNone
            ItemAppearance.InfoFillDisabled.Rounding = 0
            ItemAppearance.InfoFillDisabled.ShadowOffset = 0
            ItemAppearance.InfoFillDisabled.Glow = gmNone
            ItemAppearance.DeleteButtonCaption = 'Delete'
            ItemAppearance.DeleteButtonFont.Charset = DEFAULT_CHARSET
            ItemAppearance.DeleteButtonFont.Color = clWhite
            ItemAppearance.DeleteButtonFont.Height = -11
            ItemAppearance.DeleteButtonFont.Name = 'Tahoma'
            ItemAppearance.DeleteButtonFont.Style = [fsBold]
            LookupBar.DisabledFont.Charset = DEFAULT_CHARSET
            LookupBar.DisabledFont.Color = clSilver
            LookupBar.DisabledFont.Height = -11
            LookupBar.DisabledFont.Name = 'Tahoma'
            LookupBar.DisabledFont.Style = []
            LookupBar.Font.Charset = DEFAULT_CHARSET
            LookupBar.Font.Color = clWindowText
            LookupBar.Font.Height = -11
            LookupBar.Font.Name = 'Tahoma'
            LookupBar.Font.Style = []
            LookupBar.Visible = False
            Sections.Font.Charset = DEFAULT_CHARSET
            Sections.Font.Color = clWindowText
            Sections.Font.Height = -11
            Sections.Font.Name = 'Tahoma'
            Sections.Font.Style = []
            Sections.BorderColor = clBlack
            SelectedItemIndex = -1
            Header.Fill.Color = clNone
            Header.Fill.ColorTo = clNone
            Header.Fill.ColorMirror = clNone
            Header.Fill.ColorMirrorTo = clNone
            Header.Fill.GradientType = gtVertical
            Header.Fill.GradientMirrorType = gtSolid
            Header.Fill.BorderColor = clNone
            Header.Fill.Rounding = 0
            Header.Fill.ShadowColor = clNone
            Header.Fill.ShadowOffset = 0
            Header.Fill.Glow = gmNone
            Header.Height = 25
            Header.Caption = 'Lua scripts'
            Header.CaptionURLColor = clNavy
            Header.Font.Charset = DEFAULT_CHARSET
            Header.Font.Color = clWhite
            Header.Font.Height = -13
            Header.Font.Name = 'Tahoma'
            Header.Font.Style = []
            Filter.Fill.Color = 16773091
            Filter.Fill.ColorMirror = clNone
            Filter.Fill.ColorMirrorTo = clNone
            Filter.Fill.GradientType = gtSolid
            Filter.Fill.GradientMirrorType = gtSolid
            Filter.Fill.BorderColor = 16765615
            Filter.Fill.Rounding = 0
            Filter.Fill.ShadowOffset = 0
            Filter.Fill.Glow = gmNone
            Footer.Fill.Color = 16773091
            Footer.Fill.ColorTo = 16765615
            Footer.Fill.ColorMirror = clNone
            Footer.Fill.ColorMirrorTo = clNone
            Footer.Fill.GradientType = gtVertical
            Footer.Fill.GradientMirrorType = gtSolid
            Footer.Fill.BorderColor = 16765615
            Footer.Fill.BorderWidth = 2
            Footer.Fill.Rounding = 8
            Footer.Fill.RoundingType = rtTop
            Footer.Fill.ShadowOffset = 3
            Footer.Fill.ShadowType = stSurround
            Footer.Fill.Glow = gmNone
            Footer.Height = 25
            Footer.Caption = 'Footer'
            Footer.Font.Charset = DEFAULT_CHARSET
            Footer.Font.Color = 7485192
            Footer.Font.Height = -13
            Footer.Font.Name = 'Tahoma'
            Footer.Font.Style = []
            Footer.Visible = False
            OnItemSelected = listScriptsItemSelected
            DefaultItem.Caption = 'Item 0'
            DefaultItem.CaptionFont.Charset = DEFAULT_CHARSET
            DefaultItem.CaptionFont.Color = clSilver
            DefaultItem.CaptionFont.Height = -11
            DefaultItem.CaptionFont.Name = 'Tahoma'
            DefaultItem.CaptionFont.Style = []
            DefaultItem.CaptionSelectedFont.Charset = DEFAULT_CHARSET
            DefaultItem.CaptionSelectedFont.Color = clWindowText
            DefaultItem.CaptionSelectedFont.Height = -11
            DefaultItem.CaptionSelectedFont.Name = 'Tahoma'
            DefaultItem.CaptionSelectedFont.Style = []
            DefaultItem.InfoFont.Charset = DEFAULT_CHARSET
            DefaultItem.InfoFont.Color = clWindowText
            DefaultItem.InfoFont.Height = -11
            DefaultItem.InfoFont.Name = 'Tahoma'
            DefaultItem.InfoFont.Style = []
            DefaultItem.InfoSelectedFont.Charset = DEFAULT_CHARSET
            DefaultItem.InfoSelectedFont.Color = clWindowText
            DefaultItem.InfoSelectedFont.Height = -11
            DefaultItem.InfoSelectedFont.Name = 'Tahoma'
            DefaultItem.InfoSelectedFont.Style = []
            DefaultItem.NotesFont.Charset = DEFAULT_CHARSET
            DefaultItem.NotesFont.Color = clWindowText
            DefaultItem.NotesFont.Height = -11
            DefaultItem.NotesFont.Name = 'Tahoma'
            DefaultItem.NotesFont.Style = []
            DefaultItem.NotesSelectedFont.Charset = DEFAULT_CHARSET
            DefaultItem.NotesSelectedFont.Color = clWindowText
            DefaultItem.NotesSelectedFont.Height = -11
            DefaultItem.NotesSelectedFont.Name = 'Tahoma'
            DefaultItem.NotesSelectedFont.Style = []
            DefaultItem.ProgressMaximum = 100.000000000000000000
            Categories = <>
            Align = alClient
            TabOrder = 0
            TMSStyle = 0
          end
        end
        object panRightBot: TPanel
          Left = 0
          Top = 576
          Width = 274
          Height = 31
          Align = alBottom
          BevelOuter = bvNone
          BorderStyle = bsSingle
          TabOrder = 1
          object btnExecute: TSpeedButton
            Left = 30
            Top = 3
            Width = 75
            Height = 23
            Caption = 'Execute'
            OnClick = btnExecuteClick
          end
          object btnReload: TSpeedButton
            Left = 152
            Top = 3
            Width = 90
            Height = 23
            Caption = 'Reload script'
            OnClick = btnReloadClick
          end
        end
        object panCentral: TPanel
          Left = 0
          Top = 273
          Width = 274
          Height = 303
          Align = alClient
          BevelOuter = bvNone
          BorderStyle = bsSingle
          TabOrder = 2
          object labParams: TLabel
            Left = 0
            Top = 0
            Width = 272
            Height = 16
            Align = alTop
            Alignment = taCenter
            Caption = 'Script parameters'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            ExplicitWidth = 103
          end
          object luaMemo: TMemo
            Left = 0
            Top = 172
            Width = 272
            Height = 129
            Align = alBottom
            Lines.Strings = (
              '1.'
              '2.'
              '3.')
            ScrollBars = ssVertical
            TabOrder = 0
          end
          object scrollBox: TScrollBox
            Left = 0
            Top = 16
            Width = 272
            Height = 156
            Align = alClient
            TabOrder = 1
          end
        end
      end
    end
    object cxTabSheet2: TcxTabSheet
      Caption = 'Settings'
      ImageIndex = 1
      object boxSelection: TcxGroupBox
        Left = 0
        Top = 0
        Align = alTop
        Caption = 'Selection setting'
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Darkroom'
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
        TabOrder = 0
        Height = 243
        Width = 276
        object chkSelRestore: TpxCheckLabel
          Left = 9
          Top = 52
          Width = 237
          Height = 22
          CheckBoxSize = 14
          SmoothText = False
          PenWidth = 1
          PenCurvature = 2
          ColorBrush = 7131391
          CaptionX = 20
          CaptionY = 4
          Checked = True
          Caption = 'Restore original selection after action'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWhite
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
        end
        object Label1: TLabel
          Left = 74
          Top = 80
          Width = 121
          Height = 14
          Caption = 'Magic wand setting'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clSilver
          Font.Height = -12
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object chkSelMaxFilter: TpxCheckLabel
          Left = 9
          Top = 139
          Width = 246
          Height = 22
          CheckBoxSize = 14
          SmoothText = False
          PenWidth = 1
          PenCurvature = 2
          ColorBrush = 7131391
          CaptionX = 20
          CaptionY = 4
          Checked = True
          Caption = 'Use maximum filter to remove black holes'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWhite
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
        end
        object editSelFeather: TpxSpinSlider
          Left = 5
          Top = 22
          Width = 265
          Height = 21
          LayoutStyle = lyPixopedia
          SliderThickness = 8
          SliderLength = 110
          MaxInt = 30
          MaxFloat = 100.000000000000000000
          FloatIncrementSmall = 0.200000000000000000
          FloatIncrementLarge = 10.000000000000000000
          FloatDecimals = -2
          IntIncrementSmall = 1
          IntIncrementLarge = 10
          CheckLabel.Width = 100
          CheckLabel.Height = 21
          CheckLabel.CheckBoxSize = 10
          CheckLabel.CheckBoxVisible = False
          CheckLabel.SmoothText = False
          CheckLabel.PenWidth = 1
          CheckLabel.CaptionX = 3
          CheckLabel.CaptionY = 3
          CheckLabel.Shape = chkSemiLeft
          CheckLabel.Caption = 'Feather radius'
          CheckLabel.Font.Charset = DEFAULT_CHARSET
          CheckLabel.Font.Color = clWhite
          CheckLabel.Font.Height = -11
          CheckLabel.Font.Name = 'Tahoma'
          CheckLabel.Font.Style = []
          PenWidth = 1
          ColorEdit = 2105376
          ParentBackground = True
        end
        object editWandTol: TpxSpinSlider
          Left = 5
          Top = 111
          Width = 265
          Height = 21
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWhite
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          LayoutStyle = lyPixopedia
          SliderThickness = 8
          SliderLength = 110
          MaxInt = 255
          MaxFloat = 100.000000000000000000
          FloatIncrementSmall = 0.200000000000000000
          FloatIncrementLarge = 10.000000000000000000
          FloatDecimals = -2
          IntIncrementSmall = 1
          IntIncrementLarge = 10
          CheckLabel.Width = 100
          CheckLabel.Height = 21
          CheckLabel.CheckBoxSize = 10
          CheckLabel.CheckBoxVisible = False
          CheckLabel.SmoothText = False
          CheckLabel.PenWidth = 1
          CheckLabel.CaptionX = 3
          CheckLabel.CaptionY = 3
          CheckLabel.Shape = chkSemiLeft
          CheckLabel.Caption = 'Color tolerance'
          PenWidth = 1
          ColorEdit = 2105376
          ParentBackground = True
        end
        object rgpWandModes: TcxRadioGroup
          AlignWithMargins = True
          Left = 6
          Top = 175
          Align = alBottom
          Caption = ' Magic wand modes '
          ParentFont = False
          Properties.Columns = 3
          Properties.Items = <
            item
              Caption = 'Inclusive'
            end
            item
              Caption = 'Exclusive'
            end
            item
              Caption = 'Global'
            end>
          ItemIndex = 0
          Style.Font.Charset = DEFAULT_CHARSET
          Style.Font.Color = clSilver
          Style.Font.Height = -11
          Style.Font.Name = 'Tahoma'
          Style.Font.Style = []
          Style.LookAndFeel.NativeStyle = False
          Style.LookAndFeel.SkinName = 'Darkroom'
          Style.IsFontAssigned = True
          StyleDisabled.LookAndFeel.NativeStyle = False
          StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
          TabOrder = 2
          Height = 56
          Width = 264
        end
      end
      object boxImages: TcxGroupBox
        Left = 0
        Top = 243
        Align = alTop
        Caption = 'Auxiliary images setting'
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Darkroom'
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
        TabOrder = 1
        Height = 224
        Width = 276
        object Label2: TLabel
          Left = 20
          Top = 26
          Width = 127
          Height = 13
          Caption = 'Background tiles directory '
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clSilver
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object Label3: TLabel
          Left = 20
          Top = 90
          Width = 115
          Height = 13
          Caption = 'Color textures directory'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clSilver
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object comboShellTiles: TcxShellComboBox
          Left = 16
          Top = 50
          Properties.ReadOnly = False
          Properties.ShowFullPath = sfpAlways
          Style.LookAndFeel.NativeStyle = False
          Style.LookAndFeel.SkinName = 'Darkroom'
          StyleDisabled.LookAndFeel.NativeStyle = False
          StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
          StyleFocused.LookAndFeel.NativeStyle = False
          StyleFocused.LookAndFeel.SkinName = 'Darkroom'
          StyleHot.LookAndFeel.NativeStyle = False
          StyleHot.LookAndFeel.SkinName = 'Darkroom'
          TabOrder = 0
          Width = 249
        end
        object rgpAuxViewStyle: TcxRadioGroup
          AlignWithMargins = True
          Left = 6
          Top = 156
          Align = alBottom
          Caption = 'View style'
          ParentFont = False
          Properties.Columns = 3
          Properties.Items = <
            item
              Caption = 'Image slider'
            end
            item
              Caption = 'Combo box picker'
            end>
          Properties.OnChange = rgpAuxViewStylePropertiesChange
          ItemIndex = 0
          Style.Font.Charset = DEFAULT_CHARSET
          Style.Font.Color = clSilver
          Style.Font.Height = -11
          Style.Font.Name = 'Tahoma'
          Style.Font.Style = []
          Style.LookAndFeel.NativeStyle = False
          Style.LookAndFeel.SkinName = 'Darkroom'
          Style.IsFontAssigned = True
          StyleDisabled.LookAndFeel.NativeStyle = False
          StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
          TabOrder = 1
          Height = 56
          Width = 264
        end
        object comboShellTextures: TcxShellComboBox
          Left = 16
          Top = 114
          Properties.ReadOnly = False
          Properties.ShowFullPath = sfpAlways
          Style.LookAndFeel.NativeStyle = False
          Style.LookAndFeel.SkinName = 'Darkroom'
          StyleDisabled.LookAndFeel.NativeStyle = False
          StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
          StyleFocused.LookAndFeel.NativeStyle = False
          StyleFocused.LookAndFeel.SkinName = 'Darkroom'
          StyleHot.LookAndFeel.NativeStyle = False
          StyleHot.LookAndFeel.SkinName = 'Darkroom'
          TabOrder = 2
          Width = 249
        end
      end
      object rgpFileViewStyle: TcxRadioGroup
        AlignWithMargins = True
        Left = 3
        Top = 470
        Align = alTop
        Caption = 'File view style'
        ParentFont = False
        Properties.Columns = 3
        Properties.Items = <
          item
            Caption = 'File list'
          end
          item
            Caption = 'Shell combo box'
          end>
        Properties.OnChange = rgpAuxViewStylePropertiesChange
        ItemIndex = 0
        Style.Font.Charset = DEFAULT_CHARSET
        Style.Font.Color = clSilver
        Style.Font.Height = -11
        Style.Font.Name = 'Tahoma'
        Style.Font.Style = []
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Darkroom'
        Style.IsFontAssigned = True
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
        TabOrder = 2
        Height = 56
        Width = 270
      end
    end
  end
  object panStatus: TPanel
    Left = 0
    Top = 643
    Width = 1062
    Height = 31
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    DesignSize = (
      1062
      31)
    object labStatus: TLabel
      Left = 379
      Top = 7
      Width = 53
      Height = 16
      Anchors = [akLeft, akTop, akRight]
      Caption = 'labStatus'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object btnClearAll: TSpeedButton
      Left = 11
      Top = 5
      Width = 75
      Height = 21
      Caption = 'Clear all'
      OnClick = btnClearAllClick
    end
    object labInfo: TLabel
      Left = 813
      Top = 6
      Width = 39
      Height = 16
      Anchors = [akTop, akRight]
      Caption = 'labInfo'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
  end
  object openDialog: TOpenImageEnDialog
    Ctl3D = False
    Filter = 
      'Zajedni'#269'ke grafi'#269'ke datoteke|*.tif;*.tiff;*.fax;*.g3n;*.g3f;*.xi' +
      'f;*.gif;*.jpg;*.jpeg;*.jpe;*.jif;*.pcx;*.bmp;*.dib;*.rle;*.ico;*' +
      '.cur;*.png;*.dcm;*.dic;*.dicom;*.v2;*.wmf;*.emf;*.tga;*.targa;*.' +
      'vda;*.icb;*.vst;*.pix;*.pxm;*.ppm;*.pgm;*.pbm;*.wbmp;*.jp2;*.j2k' +
      ';*.jpc;*.j2c;*.dcx;*.crw;*.cr2;*.dng;*.nef;*.raw;*.raf;*.x3f;*.o' +
      'rf;*.srf;*.mrw;*.dcr;*.bay;*.pef;*.sr2;*.arw;*.kdc;*.mef;*.3fr;*' +
      '.k25;*.erf;*.cam;*.cs1;*.dc2;*.dcs;*.fff;*.mdc;*.mos;*.nrw;*.ptx' +
      ';*.pxn;*.rdc;*.rw2;*.rwl;*.iiq;*.srw;*.psd;*.psb;*.iev;*.lyr;*.a' +
      'll;*.wdp;*.hdp;*.jxr;*.avi;*.mpeg;*.mpg;*.wmv|Sve datoteke (*.*)' +
      '|*.*|JPEG Image (*.jpg;*.jpeg;*.jpe;*.jif)|*.jpg;*.jpeg;*.jpe;*.' +
      'jif|TIFF Image (*.tif;*.tiff;*.fax;*.g3n;*.g3f;*.xif)|*.tif;*.ti' +
      'ff;*.fax;*.g3n;*.g3f;*.xif|GIF Image (*.gif)|*.gif|PaintBrush (*' +
      '.pcx)|*.pcx|Windows Bitmap (*.bmp;*.dib;*.rle)|*.bmp;*.dib;*.rle' +
      '|Windows Icon (*.ico)|*.ico|Windows Cursor (*.cur)|*.cur|Portabl' +
      'e Network Graphics (*.png)|*.png|DICOM Medical Image (*.dcm;*.di' +
      'c;*.dicom;*.v2)|*.dcm;*.dic;*.dicom;*.v2|Windows Metafile (*.wmf' +
      ')|*.wmf|Enhanced Windows Metafile (*.emf)|*.emf|Targa Image (*.t' +
      'ga;*.targa;*.vda;*.icb;*.vst;*.pix)|*.tga;*.targa;*.vda;*.icb;*.' +
      'vst;*.pix|Portable Pixmap, Graymap, Bitmap (*.pxm;*.ppm;*.pgm;*.' +
      'pbm)|*.pxm;*.ppm;*.pgm;*.pbm|Wireless Bitmap (*.wbmp)|*.wbmp|JPE' +
      'G2000 (*.jp2)|*.jp2|JPEG2000 Code Stream (*.j2k;*.jpc;*.j2c)|*.j' +
      '2k;*.jpc;*.j2c|Multipage PCX (*.dcx)|*.dcx|Camera Raw Image (*.c' +
      'r2;*.crw;*.dng;*.nef;*.raw;*.orf;*.nrw...)|*.crw;*.cr2;*.dng;*.n' +
      'ef;*.raw;*.raf;*.x3f;*.orf;*.srf;*.mrw;*.dcr;*.bay;*.pef;*.sr2;*' +
      '.arw;*.kdc;*.mef;*.3fr;*.k25;*.erf;*.cam;*.cs1;*.dc2;*.dcs;*.fff' +
      ';*.mdc;*.mos;*.nrw;*.ptx;*.pxn;*.rdc;*.rw2;*.rwl;*.iiq;*.srw|Pho' +
      'toshop PSD (*.psd;*.psb)|*.psd;*.psb|Vectorial Objects (*.iev)|*' +
      '.iev|Layers (*.lyr)|*.lyr|Layers and Objects (*.all)|*.all|Micro' +
      'soft HD Photo (*.wdp;*.hdp;*.jxr)|*.wdp;*.hdp;*.jxr|Video za win' +
      'dows (*.avi)|*.avi|MPEG Video (*.mpeg;*.mpg)|*.mpeg;*.mpg|Window' +
      's Media Video (*.wmv)|*.wmv'
    Left = 32
    Top = 16
  end
  object saveDialog: TSaveImageEnDialog
    Ctl3D = False
    Filter = 
      'JPEG Image (*.jpg;*.jpeg;*.jpe;*.jif)|*.jpg;*.jpeg;*.jpe;*.jif|T' +
      'IFF Image (*.tif;*.tiff;*.fax;*.g3n;*.g3f;*.xif)|*.tif;*.tiff;*.' +
      'fax;*.g3n;*.g3f;*.xif|GIF Image (*.gif)|*.gif|PaintBrush (*.pcx)' +
      '|*.pcx|Windows Bitmap (*.bmp;*.dib;*.rle)|*.bmp;*.dib;*.rle|Wind' +
      'ows Icon (*.ico)|*.ico|Portable Network Graphics (*.png)|*.png|D' +
      'ICOM Medical Image (*.dcm;*.dic;*.dicom;*.v2)|*.dcm;*.dic;*.dico' +
      'm;*.v2|Targa Image (*.tga;*.targa;*.vda;*.icb;*.vst;*.pix)|*.tga' +
      ';*.targa;*.vda;*.icb;*.vst;*.pix|Portable Pixmap, Graymap, Bitma' +
      'p (*.pxm;*.ppm;*.pgm;*.pbm)|*.pxm;*.ppm;*.pgm;*.pbm|Wireless Bit' +
      'map (*.wbmp)|*.wbmp|JPEG2000 (*.jp2)|*.jp2|JPEG2000 Code Stream ' +
      '(*.j2k;*.jpc;*.j2c)|*.j2k;*.jpc;*.j2c|PostScript (*.ps;*.eps)|*.' +
      'ps;*.eps|Adobe PDF (*.pdf)|*.pdf|Multipage PCX (*.dcx)|*.dcx|Pho' +
      'toshop PSD (*.psd;*.psb)|*.psd;*.psb|Vectorial Objects (*.iev)|*' +
      '.iev|Layers (*.lyr)|*.lyr|Layers and Objects (*.all)|*.all|Micro' +
      'soft HD Photo (*.wdp;*.hdp;*.jxr)|*.wdp;*.hdp;*.jxr|Video za win' +
      'dows (*.avi)|*.avi'
    Left = 88
    Top = 16
  end
  object MainMenu1: TMainMenu
    Left = 152
    Top = 16
    object File1: TMenuItem
      Caption = '   File   '
      object itemOpenST: TMenuItem
        Caption = 'Open source && target image'
        OnClick = itemOpenSTClick
      end
      object itemOpenS: TMenuItem
        Caption = 'Open source image'
        OnClick = itemOpenSClick
      end
      object itemOpenT: TMenuItem
        Caption = 'Open target image'
        OnClick = itemOpenTClick
      end
      object itemSave: TMenuItem
        Caption = 'Save'
        OnClick = itemSaveClick
      end
    end
    object Selection1: TMenuItem
      Caption = '   Selection   '
      object ItemSelectRectangle: TMenuItem
        Caption = 'Rectangle'
        RadioItem = True
        OnClick = ItemSelectionClick
      end
      object itemSelEllipse: TMenuItem
        Tag = 1
        Caption = 'Ellipse'
        RadioItem = True
        OnClick = ItemSelectionClick
      end
      object itemLasso: TMenuItem
        Tag = 2
        Caption = 'Lasso'
        RadioItem = True
        OnClick = ItemSelectionClick
      end
      object itemPolygon: TMenuItem
        Tag = 3
        Caption = 'Polygon'
        RadioItem = True
        OnClick = ItemSelectionClick
      end
      object itemMagicWand: TMenuItem
        Tag = 4
        Caption = 'Magic wand'
        RadioItem = True
        OnClick = ItemSelectionClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object itemClearSelection: TMenuItem
        Caption = 'Clear selection'
        OnClick = itemClearSelectionClick
      end
      object itemInvertSelection: TMenuItem
        Caption = 'Invert selection'
        OnClick = itemInvertSelectionClick
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object itemSelected2Buffer: TMenuItem
        Caption = 'Copy selected area to buffer'
        OnClick = itemSelected2BufferClick
      end
    end
  end
  object popupCopy: TPopupMenu
    Left = 209
    Top = 16
    object itemCopy0: TMenuItem
      Caption = 'Copy selected image to source'
      OnClick = itemCopyClick
    end
    object copyItem1: TMenuItem
      Tag = 1
      Caption = 'Copy selected image to target'
      OnClick = itemCopyClick
    end
    object itemCopy2: TMenuItem
      Tag = 2
      Caption = 'Copy source to target'
      OnClick = itemCopyClick
    end
    object itemCopy3: TMenuItem
      Tag = 3
      Caption = 'Copy target to source'
      OnClick = itemCopyClick
    end
    object itemCopy4: TMenuItem
      Tag = 4
      Caption = 'Copy selected image as new additional image'
      OnClick = itemCopyClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object itemSwap0: TMenuItem
      Tag = 10
      Caption = 'Swap selected image and source'
      OnClick = itemCopyClick
    end
    object itemSwap1: TMenuItem
      Tag = 11
      Caption = 'Swap selected imag and target'
      OnClick = itemCopyClick
    end
    object itemSwap2: TMenuItem
      Tag = 12
      Caption = 'Swap source and target'
      OnClick = itemCopyClick
    end
  end
end
