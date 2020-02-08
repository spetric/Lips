object FrameImageCombo: TFrameImageCombo
  Left = 0
  Top = 0
  Width = 281
  Height = 60
  TabOrder = 0
  object box: TcxGroupBox
    Left = 0
    Top = 0
    Align = alClient
    Caption = 'Image parameter'
    Style.LookAndFeel.NativeStyle = False
    Style.LookAndFeel.SkinName = 'Darkroom'
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
    TabOrder = 0
    Height = 60
    Width = 281
    object image: TAdvSmoothImageListBoxPicker
      Left = 44
      Top = 21
      Width = 193
      Height = 21
      Flat = False
      FocusColor = clBtnFace
      LabelFont.Charset = DEFAULT_CHARSET
      LabelFont.Color = clWindowText
      LabelFont.Height = -11
      LabelFont.Name = 'Tahoma'
      LabelFont.Style = []
      Lookup.Separator = ';'
      Color = clBtnFace
      Enabled = True
      ReadOnly = True
      TabOrder = 0
      Text = ''
      Visible = True
      Version = '1.0.2.0'
      ButtonStyle = bsDropDown
      ButtonWidth = 16
      Etched = False
      Glyph.Data = {
        DA020000424DDA0200000000000036000000280000000D0000000D0000000100
        200000000000A402000000000000000000000000000000000000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F00000000000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000000000000000000000000000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F0000000000000000000000000000000000000000000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F0000000000000000000000000000000
        0000000000000000000000000000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0
        F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000F0F0F000}
      HideListBoxAfterSelection = True
      OnSelectImage = imageSelectImage
      SelectedImageIndex = -1
      ListBox.Left = 0
      ListBox.Top = 0
      ListBox.Width = 600
      ListBox.Height = 450
      ListBox.ScrollType = stNormal
      ListBox.ZoomAnimationFactor = 1.500000000000000000
      ListBox.Items = <>
      ListBox.TopLayerItems = <>
      ListBox.ItemAppearance.ImageWidth = 96
      ListBox.ItemAppearance.ImageHeight = 96
      ListBox.ItemAppearance.ItemWidth = 120
      ListBox.ItemAppearance.ItemHeight = 120
      ListBox.ItemAppearance.Fill.Color = 3552822
      ListBox.ItemAppearance.Fill.ColorTo = clNone
      ListBox.ItemAppearance.Fill.ColorMirror = clNone
      ListBox.ItemAppearance.Fill.ColorMirrorTo = clNone
      ListBox.ItemAppearance.Fill.GradientType = gtVertical
      ListBox.ItemAppearance.Fill.GradientMirrorType = gtVertical
      ListBox.ItemAppearance.Fill.BorderColor = 4473924
      ListBox.ItemAppearance.Fill.Rounding = 2
      ListBox.ItemAppearance.Fill.ShadowOffset = 0
      ListBox.ItemAppearance.Fill.Glow = gmNone
      ListBox.ItemAppearance.SelectedFill.Color = 4473924
      ListBox.ItemAppearance.SelectedFill.ColorTo = clNone
      ListBox.ItemAppearance.SelectedFill.ColorMirror = clNone
      ListBox.ItemAppearance.SelectedFill.ColorMirrorTo = clNone
      ListBox.ItemAppearance.SelectedFill.GradientType = gtVertical
      ListBox.ItemAppearance.SelectedFill.GradientMirrorType = gtVertical
      ListBox.ItemAppearance.SelectedFill.BorderColor = 4473924
      ListBox.ItemAppearance.SelectedFill.Rounding = 2
      ListBox.ItemAppearance.SelectedFill.ShadowOffset = 0
      ListBox.ItemAppearance.SelectedFill.Glow = gmNone
      ListBox.ItemAppearance.SelectedFill.GlowGradientColor = clNone
      ListBox.ItemAppearance.DisabledFill.Color = 3552822
      ListBox.ItemAppearance.DisabledFill.ColorTo = clNone
      ListBox.ItemAppearance.DisabledFill.ColorMirror = clNone
      ListBox.ItemAppearance.DisabledFill.ColorMirrorTo = clNone
      ListBox.ItemAppearance.DisabledFill.GradientType = gtVertical
      ListBox.ItemAppearance.DisabledFill.GradientMirrorType = gtVertical
      ListBox.ItemAppearance.DisabledFill.BorderColor = 4473924
      ListBox.ItemAppearance.DisabledFill.Rounding = 2
      ListBox.ItemAppearance.DisabledFill.ShadowOffset = 0
      ListBox.ItemAppearance.DisabledFill.Glow = gmNone
      ListBox.ItemAppearance.HoverFill.Color = 6974058
      ListBox.ItemAppearance.HoverFill.ColorTo = 6974058
      ListBox.ItemAppearance.HoverFill.ColorMirror = clNone
      ListBox.ItemAppearance.HoverFill.ColorMirrorTo = clNone
      ListBox.ItemAppearance.HoverFill.GradientType = gtVertical
      ListBox.ItemAppearance.HoverFill.GradientMirrorType = gtVertical
      ListBox.ItemAppearance.HoverFill.BorderColor = 6974058
      ListBox.ItemAppearance.HoverFill.Rounding = 2
      ListBox.ItemAppearance.HoverFill.ShadowOffset = 0
      ListBox.ItemAppearance.HoverFill.Glow = gmNone
      ListBox.ItemAppearance.HoverFill.GlowGradientColor = 14285309
      ListBox.ItemAppearance.Splitter.Fill.Color = 5723991
      ListBox.ItemAppearance.Splitter.Fill.ColorTo = 5723991
      ListBox.ItemAppearance.Splitter.Fill.ColorMirror = clNone
      ListBox.ItemAppearance.Splitter.Fill.ColorMirrorTo = clNone
      ListBox.ItemAppearance.Splitter.Fill.GradientType = gtVertical
      ListBox.ItemAppearance.Splitter.Fill.GradientMirrorType = gtSolid
      ListBox.ItemAppearance.Splitter.Fill.BorderColor = 5723991
      ListBox.ItemAppearance.Splitter.Fill.Rounding = 0
      ListBox.ItemAppearance.Splitter.Fill.ShadowOffset = 0
      ListBox.ItemAppearance.Splitter.Fill.Glow = gmNone
      ListBox.ItemAppearance.Splitter.TextFont.Charset = DEFAULT_CHARSET
      ListBox.ItemAppearance.Splitter.TextFont.Color = clWindowText
      ListBox.ItemAppearance.Splitter.TextFont.Height = -11
      ListBox.ItemAppearance.Splitter.TextFont.Name = 'Tahoma'
      ListBox.ItemAppearance.Splitter.TextFont.Style = []
      ListBox.ItemAppearance.Splitter.ExpanderColor = 3552822
      ListBox.ItemAppearance.Splitter.ExpanderBorderColor = 4473924
      ListBox.ItemAppearance.Splitter.ExpanderDownColor = 4473924
      ListBox.ItemAppearance.Splitter.ExpanderHoverColor = 6974058
      ListBox.Header.Caption = 'Select image'
      ListBox.Header.Font.Charset = DEFAULT_CHARSET
      ListBox.Header.Font.Color = 15790320
      ListBox.Header.Font.Height = -13
      ListBox.Header.Font.Name = 'Tahoma'
      ListBox.Header.Font.Style = []
      ListBox.Header.Height = 30
      ListBox.Header.Fill.Color = 4473924
      ListBox.Header.Fill.ColorTo = clNone
      ListBox.Header.Fill.ColorMirror = clNone
      ListBox.Header.Fill.ColorMirrorTo = clNone
      ListBox.Header.Fill.GradientType = gtVertical
      ListBox.Header.Fill.GradientMirrorType = gtSolid
      ListBox.Header.Fill.BorderColor = 4473924
      ListBox.Header.Fill.Rounding = 0
      ListBox.Header.Fill.ShadowOffset = 0
      ListBox.Header.Fill.Glow = gmNone
      ListBox.Header.Navigator.Visible = False
      ListBox.Header.Navigator.Color = 3552822
      ListBox.Header.Navigator.HintNext = 'Next Item'
      ListBox.Header.Navigator.HintPrevious = 'Previous Item'
      ListBox.Header.Navigator.HintNextPage = 'Next Page'
      ListBox.Header.Navigator.HintPreviousPage = 'Previous Page'
      ListBox.Header.Navigator.DisabledColor = clGray
      ListBox.Header.Navigator.HoverColor = 6974058
      ListBox.Header.Navigator.DownColor = 7067902
      ListBox.Header.Navigator.BorderColor = 4473924
      ListBox.Footer.Caption = 'Footer'
      ListBox.Footer.Font.Charset = DEFAULT_CHARSET
      ListBox.Footer.Font.Color = 15790320
      ListBox.Footer.Font.Height = -13
      ListBox.Footer.Font.Name = 'Tahoma'
      ListBox.Footer.Font.Style = []
      ListBox.Footer.Height = 30
      ListBox.Footer.Fill.Color = 4473924
      ListBox.Footer.Fill.ColorTo = clNone
      ListBox.Footer.Fill.ColorMirror = clNone
      ListBox.Footer.Fill.ColorMirrorTo = clNone
      ListBox.Footer.Fill.GradientType = gtVertical
      ListBox.Footer.Fill.GradientMirrorType = gtSolid
      ListBox.Footer.Fill.BorderColor = 4473924
      ListBox.Footer.Fill.Rounding = 0
      ListBox.Footer.Fill.ShadowOffset = 0
      ListBox.Footer.Fill.Glow = gmNone
      ListBox.Footer.Navigator.Visible = True
      ListBox.Footer.Navigator.Color = 3552822
      ListBox.Footer.Navigator.HintNext = 'Next Item'
      ListBox.Footer.Navigator.HintPrevious = 'Previous Item'
      ListBox.Footer.Navigator.HintNextPage = 'Next Page'
      ListBox.Footer.Navigator.HintPreviousPage = 'Previous Page'
      ListBox.Footer.Navigator.DisabledColor = clGray
      ListBox.Footer.Navigator.HoverColor = 6974058
      ListBox.Footer.Navigator.DownColor = 4473924
      ListBox.Footer.Navigator.BorderColor = 4473924
      ListBox.Fill.Color = 3552822
      ListBox.Fill.ColorTo = clNone
      ListBox.Fill.ColorMirror = clNone
      ListBox.Fill.ColorMirrorTo = clNone
      ListBox.Fill.GradientType = gtSolid
      ListBox.Fill.GradientMirrorType = gtSolid
      ListBox.Fill.BorderColor = clNone
      ListBox.Fill.Rounding = 0
      ListBox.Fill.ShadowOffset = 0
      ListBox.Fill.Glow = gmNone
      ListBox.DefaultHTMLText.Location = cpTopLeft
      ListBox.DefaultHTMLText.Font.Charset = DEFAULT_CHARSET
      ListBox.DefaultHTMLText.Font.Color = clWindowText
      ListBox.DefaultHTMLText.Font.Height = -11
      ListBox.DefaultHTMLText.Font.Name = 'Tahoma'
      ListBox.DefaultHTMLText.Font.Style = []
      ListBox.Columns = 4
      ListBox.Rows = 0
      ListBox.TabOrder = 0
      ListBox.TMSStyle = 23
      TMSStyle = 23
    end
  end
end
