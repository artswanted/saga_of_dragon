// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;

using SegmentedTrackBarDLL;
using NiManagedToolInterface;

namespace AnimationToolUI
{
	/// <summary>
	/// Summary description for TimeSlider.
	/// </summary>
    public class TimeSlider : System.Windows.Forms.UserControl
    {
        private const string strFullSeq = "Full Sequence: ";
        private bool m_bMultiSelect = false;

        internal MTransition m_Transition = null;
        internal MSequence m_SrcSeq = null;
        internal MSequence m_DestSeq = null;
        internal float m_fSrcSeqLength;
        internal float m_fDestSeqLength;
        internal bool m_bRestartPlaying = false;

        private float m_fTotalTime = 0.0f;
        internal float m_fTrackRangeOverTotalTime = 0.0f;
        internal float TotalTime
        {
            get { return m_fTotalTime; }
            set 
            {
                m_fTotalTime = value; 

                if (m_fTotalTime > 0.0f)
                    m_fTrackRangeOverTotalTime = (float)m_iTrackRange / m_fTotalTime;
            }
        }
        internal float TrackRangeOverTotalTime
        {
            get { return m_fTrackRangeOverTotalTime; }
        }

        private TimeSliderTransitionInfo m_Info = null;

        private enum ImageIndex
        {
            Play = 0,
            Pause = 1,
        };

        SegmentedTrackBarDLL.Events.EventHandler_ScrubberPosChanged m_HandlerScrubberPosChanged;
        System.Windows.Forms.PaintEventHandler m_HandlerSegTrkTimePaint;

        internal int m_iTrackRange;
        internal float m_fBeginTime = 0.0f;
        internal float m_fEndTime = 0.0f;
		private bool m_bPlaying = false;
	
        private bool Playing
        {
            get
            {
                return m_bPlaying;
            }
            set
            {
                SetPlayHandler(value);

                m_bPlaying = value;
                MFramework.Instance.Clock.Enabled = m_bPlaying;
                UpdatePlayButtonState();
            }
        }
        private static uint NumInstances = 0;


        private void SetPlayHandler(bool bValue)
        {
            if (m_bPlaying != bValue)
            {
                if (bValue == true)
                {
                    SegmentedTrackBarDLL.Events.ScrubberPosChanged -=
                        m_HandlerScrubberPosChanged;
         
                    Paint += 
                        m_HandlerSegTrkTimePaint; 
                }
                else
                {
                    Paint -= 
                        m_HandlerSegTrkTimePaint; 
                    SegmentedTrackBarDLL.Events.ScrubberPosChanged +=
                        m_HandlerScrubberPosChanged;
                }
            }
        }

        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.Label lblBeginTime;
        private System.Windows.Forms.Label lblEndTime;
        private System.Windows.Forms.Label lblCurrentTime;
        private OOGroup.Windows.Forms.ImageButton btnPlay;
        private System.Windows.Forms.ToolTip ttToolTip;
        private OOGroup.Windows.Forms.ImageButton btnFirstFrame;
        private OOGroup.Windows.Forms.ImageButton btnFinalFrame;
        private OOGroup.Windows.Forms.ImageButton btnOptions;
        internal SegmentedTrackBarDLL.SegmentedTrackBar segtrkTime;

		private System.Windows.Forms.ContextMenu cmEventMenu;
		private System.Windows.Forms.MenuItem miAddEvent;
		private System.Windows.Forms.MenuItem miModifyEvent;
		private System.Windows.Forms.MenuItem miRemoveEvent;
		private System.Windows.Forms.MenuItem miRegisterMacro;

        internal System.Drawing.Color colorSrcA = System.Drawing.Color.Green;
        internal System.Drawing.Color colorSrcB = System.Drawing.Color.LightGreen;
        internal System.Drawing.Color colorDurA = System.Drawing.Color.Yellow;
        internal System.Drawing.Color colorDurB = System.Drawing.Color.Yellow;       
        internal System.Drawing.Color colorDestA = System.Drawing.Color.Red;
        internal System.Drawing.Color colorDestB = System.Drawing.Color.Pink;
        private OOGroup.Windows.Forms.ImageButton btnNextFrame;
        private OOGroup.Windows.Forms.ImageButton btnPrevFrame;
        private System.Windows.Forms.Label lblPlaybackRate;
        private AnimationToolUI.SliderBar sbPlaybackRate;

        private System.Windows.Forms.ImageList ilAnimControls;

		public TimeSlider()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                NumInstances--;
                Debug.Assert(NumInstances == 0, "There should only ever be one un-disposed" + 
                    " version of this class in memory at a time.");
                if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TimeSlider));
            this.lblBeginTime = new System.Windows.Forms.Label();
            this.lblEndTime = new System.Windows.Forms.Label();
            this.ilAnimControls = new System.Windows.Forms.ImageList(this.components);
            this.lblCurrentTime = new System.Windows.Forms.Label();
            this.ttToolTip = new System.Windows.Forms.ToolTip(this.components);
            this.lblPlaybackRate = new System.Windows.Forms.Label();
            this.sbPlaybackRate = new AnimationToolUI.SliderBar();
            this.btnPrevFrame = new OOGroup.Windows.Forms.ImageButton();
            this.btnNextFrame = new OOGroup.Windows.Forms.ImageButton();
            this.btnFinalFrame = new OOGroup.Windows.Forms.ImageButton();
            this.btnOptions = new OOGroup.Windows.Forms.ImageButton();
            this.btnPlay = new OOGroup.Windows.Forms.ImageButton();
            this.btnFirstFrame = new OOGroup.Windows.Forms.ImageButton();
            this.cmEventMenu = new System.Windows.Forms.ContextMenu();
            this.miAddEvent = new System.Windows.Forms.MenuItem();
            this.miModifyEvent = new System.Windows.Forms.MenuItem();
            this.miRemoveEvent = new System.Windows.Forms.MenuItem();
            this.miRegisterMacro = new System.Windows.Forms.MenuItem();
            this.segtrkTime = new SegmentedTrackBarDLL.SegmentedTrackBar();
            this.SuspendLayout();
            // 
            // lblBeginTime
            // 
            this.lblBeginTime.AutoSize = true;
            this.lblBeginTime.Location = new System.Drawing.Point(24, 72);
            this.lblBeginTime.Name = "lblBeginTime";
            this.lblBeginTime.Size = new System.Drawing.Size(58, 12);
            this.lblBeginTime.TabIndex = 7;
            this.lblBeginTime.Text = "0.000 sec";
            // 
            // lblEndTime
            // 
            this.lblEndTime.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblEndTime.AutoSize = true;
            this.lblEndTime.Location = new System.Drawing.Point(290, 72);
            this.lblEndTime.Name = "lblEndTime";
            this.lblEndTime.Size = new System.Drawing.Size(58, 12);
            this.lblEndTime.TabIndex = 9;
            this.lblEndTime.Text = "0.000 sec";
            this.lblEndTime.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // ilAnimControls
            // 
            this.ilAnimControls.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilAnimControls.ImageStream")));
            this.ilAnimControls.TransparentColor = System.Drawing.Color.Fuchsia;
            this.ilAnimControls.Images.SetKeyName(0, "");
            this.ilAnimControls.Images.SetKeyName(1, "");
            this.ilAnimControls.Images.SetKeyName(2, "");
            this.ilAnimControls.Images.SetKeyName(3, "");
            this.ilAnimControls.Images.SetKeyName(4, "");
            this.ilAnimControls.Images.SetKeyName(5, "");
            this.ilAnimControls.Images.SetKeyName(6, "");
            // 
            // lblCurrentTime
            // 
            this.lblCurrentTime.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.lblCurrentTime.AutoSize = true;
            this.lblCurrentTime.Location = new System.Drawing.Point(159, 72);
            this.lblCurrentTime.Name = "lblCurrentTime";
            this.lblCurrentTime.Size = new System.Drawing.Size(58, 12);
            this.lblCurrentTime.TabIndex = 8;
            this.lblCurrentTime.Text = "0.000 sec";
            this.lblCurrentTime.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ttToolTip
            // 
            this.ttToolTip.AutomaticDelay = 1000;
            // 
            // lblPlaybackRate
            // 
            this.lblPlaybackRate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblPlaybackRate.Location = new System.Drawing.Point(426, 72);
            this.lblPlaybackRate.Name = "lblPlaybackRate";
            this.lblPlaybackRate.Size = new System.Drawing.Size(32, 16);
            this.lblPlaybackRate.TabIndex = 11;
            this.lblPlaybackRate.Text = "0.50x";
            this.ttToolTip.SetToolTip(this.lblPlaybackRate, "Playback Rate");
            // 
            // sbPlaybackRate
            // 
            this.sbPlaybackRate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.sbPlaybackRate.BorderPadding = 3;
            this.sbPlaybackRate.DecimalPlaces = 2;
            this.sbPlaybackRate.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.sbPlaybackRate.LeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.sbPlaybackRate.Location = new System.Drawing.Point(362, 72);
            this.sbPlaybackRate.Maximum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.sbPlaybackRate.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.sbPlaybackRate.Name = "sbPlaybackRate";
            this.sbPlaybackRate.RightColor = System.Drawing.SystemColors.Control;
            this.sbPlaybackRate.Size = new System.Drawing.Size(56, 16);
            this.sbPlaybackRate.TabIndex = 10;
            this.ttToolTip.SetToolTip(this.sbPlaybackRate, "Playback Rate");
            this.sbPlaybackRate.Value = new decimal(new int[] {
            100,
            0,
            0,
            131072});
            this.sbPlaybackRate.ValueChanged += new AnimationToolUI.ValueChangedEventHandler(this.sbPlaybackRate_ValueChanged);
            // 
            // btnPrevFrame
            // 
            this.btnPrevFrame.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnPrevFrame.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnPrevFrame.ImageIndex = 3;
            this.btnPrevFrame.ImageList = this.ilAnimControls;
            this.btnPrevFrame.Location = new System.Drawing.Point(362, 8);
            this.btnPrevFrame.Name = "btnPrevFrame";
            this.btnPrevFrame.Size = new System.Drawing.Size(28, 28);
            this.btnPrevFrame.TabIndex = 1;
            this.ttToolTip.SetToolTip(this.btnPrevFrame, "Previous Frame");
            this.btnPrevFrame.Click += new System.EventHandler(this.btnPrevFrame_Click);
            // 
            // btnNextFrame
            // 
            this.btnNextFrame.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnNextFrame.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnNextFrame.ImageIndex = 5;
            this.btnNextFrame.ImageList = this.ilAnimControls;
            this.btnNextFrame.Location = new System.Drawing.Point(426, 8);
            this.btnNextFrame.Name = "btnNextFrame";
            this.btnNextFrame.Size = new System.Drawing.Size(28, 28);
            this.btnNextFrame.TabIndex = 3;
            this.ttToolTip.SetToolTip(this.btnNextFrame, "Next Frame");
            this.btnNextFrame.Click += new System.EventHandler(this.btnNextFrame_Click);
            // 
            // btnFinalFrame
            // 
            this.btnFinalFrame.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnFinalFrame.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnFinalFrame.ImageIndex = 6;
            this.btnFinalFrame.ImageList = this.ilAnimControls;
            this.btnFinalFrame.Location = new System.Drawing.Point(394, 40);
            this.btnFinalFrame.Name = "btnFinalFrame";
            this.btnFinalFrame.Size = new System.Drawing.Size(28, 28);
            this.btnFinalFrame.TabIndex = 5;
            this.ttToolTip.SetToolTip(this.btnFinalFrame, "Final Frame");
            this.btnFinalFrame.Click += new System.EventHandler(this.btnFinalFrame_Click);
            // 
            // btnOptions
            // 
            this.btnOptions.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOptions.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOptions.ImageIndex = 2;
            this.btnOptions.ImageList = this.ilAnimControls;
            this.btnOptions.Location = new System.Drawing.Point(426, 40);
            this.btnOptions.Name = "btnOptions";
            this.btnOptions.Size = new System.Drawing.Size(28, 28);
            this.btnOptions.TabIndex = 6;
            this.ttToolTip.SetToolTip(this.btnOptions, "Animation Options");
            this.btnOptions.Click += new System.EventHandler(this.btnOptions_Click);
            // 
            // btnPlay
            // 
            this.btnPlay.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnPlay.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnPlay.ImageIndex = 0;
            this.btnPlay.ImageList = this.ilAnimControls;
            this.btnPlay.Location = new System.Drawing.Point(394, 8);
            this.btnPlay.Name = "btnPlay";
            this.btnPlay.Size = new System.Drawing.Size(28, 28);
            this.btnPlay.TabIndex = 2;
            this.ttToolTip.SetToolTip(this.btnPlay, "Play");
            this.btnPlay.Click += new System.EventHandler(this.btnPlay_Click);
            // 
            // btnFirstFrame
            // 
            this.btnFirstFrame.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnFirstFrame.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnFirstFrame.ImageIndex = 4;
            this.btnFirstFrame.ImageList = this.ilAnimControls;
            this.btnFirstFrame.Location = new System.Drawing.Point(362, 40);
            this.btnFirstFrame.Name = "btnFirstFrame";
            this.btnFirstFrame.Size = new System.Drawing.Size(28, 28);
            this.btnFirstFrame.TabIndex = 4;
            this.ttToolTip.SetToolTip(this.btnFirstFrame, "First Frame");
            this.btnFirstFrame.Click += new System.EventHandler(this.btnFirstFrame_Click);
            // 
            // cmEventMenu
            // 
            this.cmEventMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.miAddEvent,
            this.miModifyEvent,
            this.miRemoveEvent});
            // 
            // miAddEvent
            // 
            this.miAddEvent.Index = 0;
            this.miAddEvent.Text = "&Add New Event";
            this.miAddEvent.Click += new System.EventHandler(this.miAddEvent_Click);
            // 
            // miModifyEvent
            // 
            this.miModifyEvent.Index = 1;
            this.miModifyEvent.Text = "&Modify Event";
            this.miModifyEvent.Click += new System.EventHandler(this.miModifyEvent_Click);
            // 
            // miRemoveEvent
            // 
            this.miRemoveEvent.Index = 2;
            this.miRemoveEvent.Text = "&Remove Event";
            this.miRemoveEvent.Click += new System.EventHandler(this.miRemoveEvent_Click);
            // 
            // miRegisterMacro
            // 
            this.miRegisterMacro.Index = -1;
            this.miRegisterMacro.Text = "&Register Macro";
            this.miRegisterMacro.Click += new System.EventHandler(this.miRegisterMacro_Click);
            // 
            // segtrkTime
            // 
            this.segtrkTime.BackColorInRange = System.Drawing.Color.DarkGray;
            this.segtrkTime.BackColorOutOfRange = System.Drawing.Color.LightGray;
            this.segtrkTime.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.segtrkTime.Increment = 10;
            this.segtrkTime.LevelHeight = 16;
            this.segtrkTime.Location = new System.Drawing.Point(8, 8);
            this.segtrkTime.MaxRange = 9999;
            this.segtrkTime.MinRange = 0;
            this.segtrkTime.Name = "segtrkTime";
            this.segtrkTime.RangeLimiterLeft.Enabled = true;
            this.segtrkTime.RangeLimiterLeft.Position = 0;
            this.segtrkTime.RangeLimiterLeft.Visible = true;
            this.segtrkTime.RangeLimiterRight.Enabled = true;
            this.segtrkTime.RangeLimiterRight.Position = 9999;
            this.segtrkTime.RangeLimiterRight.Visible = true;
            this.segtrkTime.Scrubber.Enabled = true;
            this.segtrkTime.Scrubber.Height = 32;
            this.segtrkTime.Scrubber.Position = 0;
            this.segtrkTime.Scrubber.Visible = true;
            this.segtrkTime.Scrubber.Width = 15;
            this.segtrkTime.Size = new System.Drawing.Size(354, 64);
            this.segtrkTime.TabIndex = 0;
            this.segtrkTime.Thickness = 32;
            this.segtrkTime.UseLevelDividers = true;
            // 
            // TimeSlider
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
            this.Controls.Add(this.lblPlaybackRate);
            this.Controls.Add(this.sbPlaybackRate);
            this.Controls.Add(this.btnPrevFrame);
            this.Controls.Add(this.btnNextFrame);
            this.Controls.Add(this.lblBeginTime);
            this.Controls.Add(this.lblCurrentTime);
            this.Controls.Add(this.lblEndTime);
            this.Controls.Add(this.btnFinalFrame);
            this.Controls.Add(this.btnOptions);
            this.Controls.Add(this.btnPlay);
            this.Controls.Add(this.btnFirstFrame);
            this.Controls.Add(this.segtrkTime);
            this.Name = "TimeSlider";
            this.Size = new System.Drawing.Size(466, 96);
            this.Load += new System.EventHandler(this.TimeSlider_Load);
            this.SizeChanged += new System.EventHandler(this.TimeSlider_SizeChanged);
            this.ResumeLayout(false);
            this.PerformLayout();

        }
		#endregion

        private void TimeSlider_Load(object sender, System.EventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            // Initialize range variables.
            m_iTrackRange = segtrkTime.MaxRange - segtrkTime.MinRange;

            // Initialize button images.
            btnPlay.SetImage((Bitmap) btnPlay.ImageList.Images
                [btnPlay.ImageIndex]);
            btnFirstFrame.SetImage((Bitmap) btnFirstFrame.ImageList.Images
                [btnFirstFrame.ImageIndex]);
            btnFinalFrame.SetImage((Bitmap) btnFinalFrame.ImageList.Images
                [btnFinalFrame.ImageIndex]);
            btnPrevFrame.SetImage((Bitmap) btnPrevFrame.ImageList.Images
                [btnPrevFrame.ImageIndex]);
            btnNextFrame.SetImage((Bitmap) btnNextFrame.ImageList.Images
                [btnNextFrame.ImageIndex]);
            btnOptions.SetImage((Bitmap) btnOptions.ImageList.Images
                [btnOptions.ImageIndex]);

            m_HandlerScrubberPosChanged = new SegmentedTrackBarDLL.Events
                .EventHandler_ScrubberPosChanged(OnScrubberPosChanged);
            m_HandlerSegTrkTimePaint = new System.Windows.Forms
                .PaintEventHandler(segtrkTime_Paint);

            // Initialize labels.
            UpdateTimeRangeLabels();
            Playing = true;

            // Register event handlers.
            AppEvents.TransitionEditLoaded +=
                new AnimationToolUI.AppEvents.EventHandler_TransitionEditLoaded(OnTransitionEditLoaded);

            MFramework.Instance.Animation.OnPlaybackModeChanged +=
                new MAnimation.__Delegate_OnPlaybackModeChanged(
                OnPlaybackModeChanged);

            MFramework.Instance.Animation.OnDefaultTransitionSettingsChanged +=
                new NiManagedToolInterface.MAnimation.__Delegate_OnDefaultTransitionSettingsChanged(OnDefaultTransitionSettingsChanged);

            MFramework.Instance.Animation.OnTransitionModified +=
                new MAnimation.__Delegate_OnTransitionModified(
                OnTransitionModified);

            MFramework.Instance.Clock.OnEnabledChanged  +=new NiManagedToolInterface.MTimeManager.__Delegate_OnEnabledChanged(Clock_OnEnabledChanged);
            MFramework.Instance.Clock.OnScaleFactorChanged +=new NiManagedToolInterface.MTimeManager.__Delegate_OnScaleFactorChanged(Clock_OnScaleFactorChanged);

            SegmentedTrackBarDLL.Events.MarkerPosChanged +=
                new SegmentedTrackBarDLL.Events.EventHandler_MarkerPosChanged(
                OnMarkerPosChanged);

            SegmentedTrackBarDLL.Events.MarkerDragEnded +=
                new SegmentedTrackBarDLL.Events.EventHandler_MarkerDragEnded(
                OnMarkerDragEnded);

            SegmentedTrackBarDLL.Events.MarkerDragStarted +=
                new SegmentedTrackBarDLL.Events.EventHandler_MarkerDragStarted(
                OnMarkerDragStarted);

            SegmentedTrackBarDLL.Events.ScrubberRepositioned +=
                new SegmentedTrackBarDLL.Events.EventHandler_ScrubberRepositioned(
                OnScrubberRepositioned);

			SegmentedTrackBarDLL.Events.ContextMenuClicked +=
				new SegmentedTrackBarDLL.Events.EventHandler_ContextMenu(
				OnContextMenuClicked);

			SegmentedTrackBarDLL.Events.TextKeyEventRepositioned +=
				new SegmentedTrackBarDLL.Events.EventHandler_TextKeyEventRepositioned(
				OnTextKeyEventRepositioned);

            sbPlaybackRate.SetValue(Convert.ToDecimal(MFramework.Instance.Clock.ScaleFactor));
            lblPlaybackRate.Text = 
                MFramework.Instance.Clock.ScaleFactor.ToString("f2") + "x";

            m_bMultiSelect = false;

            AppEvents.TransitionViewSelectionCountChanged +=
                new AppEvents.EventHandler_TransitionViewSelectionCountChanged(
                    this.OnTransitionViewSelectionCountChanged);

            Playing = true;
        }

        private void OnTransitionViewSelectionCountChanged(int iCount)
        {
            if (iCount > 1)
                m_bMultiSelect = true;
            else
                m_bMultiSelect = false;
        }

        private void OnDefaultTransitionSettingsChanged(
            MTransition.TransitionType eType)
        {
            OnPlaybackModeChanged(MAnimation.PlaybackMode.Transition, false);
        }

        private void OnTransitionModified(MTransition.PropertyType proptype,
            MTransition tran)
        {
            OnPlaybackModeChanged(MAnimation.PlaybackMode.Transition, false);
        }

        private void OnPlaybackModeChanged(MAnimation.PlaybackMode eMode)
        {
            OnPlaybackModeChanged(eMode, true);
        }

        private void OnPlaybackModeChanged(MAnimation.PlaybackMode eMode,
            bool bChangePlayState)
        {
            if (m_bMultiSelect)
            {
                if (eMode == MAnimation.PlaybackMode.None ||
                    eMode == MAnimation.PlaybackMode.Transition)
                {
                    if (Visible == true)
                        Visible = false;

                    return;
                }
            }

            segtrkTime.ClearRegions();
            segtrkTime.ClearMarkers();
            segtrkTime.ClearTicks();
            m_Transition = null;
            m_SrcSeq = null;
            m_DestSeq = null;

            segtrkTime.RangeLimiterRight.Position = m_iTrackRange;

            switch (eMode)
            {
                case MAnimation.PlaybackMode.None:
                case MAnimation.PlaybackMode.SequenceGroup:
                case MAnimation.PlaybackMode.Interactive:
                    if (bChangePlayState)
                    {
                        SetPlayHandler(Playing);
                    }
                    Visible = false;
                    break;
                case MAnimation.PlaybackMode.Transition:
                {
                    if (bChangePlayState)
                    {
                        SetPlayHandler(Playing);
                    }
                    SetupForTransition();
                    break;
                }
                default:
                {
                    m_Info = new TimeSliderSequence(this);
                    m_Info.Setup();
                    if (bChangePlayState)
                    {
                        SetPlayHandler(Playing);
                    }
                    Visible = true;
                    break;
                }
            }

            OnScrubberRepositioned();

            UpdateTimeRangeLabels();
            UpdatePlayButtonState();

            if (Playing != MFramework.Instance.Clock.Enabled)
            {
                MFramework.Instance.Clock.Enabled = Playing;
            }      
        }

        public void OnTransitionEditLoaded()
        {
            // When the TransitionEdit is loaded, it
            // resets animations, so we need to catch this
            // event and position the scrubber based on
            // it's previous values.
            OnScrubberRepositioned();
        }

        private void UpdateTimeRangeLabels()
        {
            lblBeginTime.Text = m_fBeginTime.ToString("f3") + " sec";
            lblEndTime.Text = m_fEndTime.ToString("f3") + " sec";
        }

        private void UpdateCurrentTimeLabel(float fCurrentTime)
        {
            lblCurrentTime.Text = fCurrentTime.ToString("f3") + " sec";
        }

        private void UpdatePlayButtonState()
        {
            if (m_bMultiSelect || Visible == false)
                return;

            if (Playing)
            {
                btnPlay.SetImage((Bitmap) ilAnimControls.Images
                    [(int) ImageIndex.Pause]);
                ttToolTip.SetToolTip(btnPlay, "Pause");

                segtrkTime.Scrubber.Enabled = false;
            }
            else
            {
                btnPlay.SetImage((Bitmap) ilAnimControls.Images
                    [(int) ImageIndex.Play]);
                ttToolTip.SetToolTip(btnPlay, "Play");

                segtrkTime.Scrubber.Enabled = true;
            }
        }

        private void OnScrubberPosChanged()
        {
            if (m_Info == null)
                return;

            m_Info.OnScrubberPosChanged();
            UpdateCurrentTimeLabel(MFramework.Instance.Animation.CurrentTime);
        }

        private void btnPlay_Click(object sender, System.EventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
                return;

            Playing = !Playing;
        }

        private void btnFirstFrame_Click(object sender, System.EventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Animation.CurrentTime = m_fBeginTime;
            segtrkTime_Paint(null, null);
        }

        private void btnFinalFrame_Click(object sender, System.EventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
                return;

            float fEndTime = m_fEndTime;
            if (MFramework.Instance.Animation.CurrentTime == m_fBeginTime)
            {
                // Subtract a miniscule amount to ensure that the update will
                // occur properly if we are currently at the start time.
                fEndTime -= 0.0001f;
            }
            MFramework.Instance.Animation.CurrentTime = fEndTime;
            segtrkTime_Paint(null, null);
        }

        private void btnOptions_Click(object sender, System.EventArgs e)
        {
            SceneOptionsForm dlgSceneOptions = new SceneOptionsForm();
            dlgSceneOptions.ShowDialog(this);
        }

        private void segtrkTime_Paint(object sender,
            System.Windows.Forms.PaintEventArgs e)
        {
            if (m_Info == null)
                return;

            m_Info.OnPaint();
            UpdateCurrentTimeLabel(MFramework.Instance.Animation.CurrentTime);        
        }

        public void SetupForTransition()
        {
            m_Transition = MFramework.Instance.Animation.ActiveTransition;

            if (m_Transition == null)
                return;

            m_SrcSeq = MFramework.Instance.Animation.GetSequence(m_Transition.SrcID);
            m_DestSeq = MFramework.Instance.Animation.GetSequence(m_Transition.DesID);

            m_fSrcSeqLength = m_SrcSeq.Length; 
            m_fDestSeqLength = m_DestSeq.Length;

            segtrkTime.SetNumberOfLevels(2);

            switch(m_Transition.Type)
            {
                case MTransition.TransitionType.Trans_Chain:
                    m_Info = new TimeSliderChain(this);
                    break;
                case MTransition.TransitionType.Trans_DelayedBlend:
                    m_Info = new TimeSliderBlendDelayed(this);
                    break;
                case MTransition.TransitionType.Trans_ImmediateBlend:
                    m_Info = new TimeSliderBlendImmediate(this);
                    break;
                case MTransition.TransitionType.Trans_Morph:
                    m_Info = new TimeSliderMorph(this);
                    break;
                case MTransition.TransitionType.Trans_CrossFade:
                    m_Info = new TimeSliderCrossFade(this);
                    break;
            }

            m_Info.Setup();
        
            Visible = true;
        }

        public SegmentedTrackBarDLL.MarkerItem GetNextSrcMarker(
            SegmentedTrackBarDLL.MarkerItem marker)
        {
            // we want to know where our marker is... what is
            // the next blend pair to the right?
            // or in otherwords, what is the next disable marker?

            SegmentedTrackBarDLL.MarkerItem nextMarker = null;
            foreach (MarkerItem m in segtrkTime.Markers)
            {
                if (m == marker)
                    continue;

                // Only want source markers... which are above trackbar
                if (m.BelowBar)
                    continue;
             
                if (m.Position >= marker.Position)
                {
                    if (nextMarker != null)
                    {
                        if (m.Position < nextMarker.Position)
                            nextMarker = m;
                    }
                    else
                        nextMarker = m;
                }
            }

            return nextMarker;
        }

        public SegmentedTrackBarDLL.MarkerItem GetNextDestMarker(
            int iFromPos)
        {
            // we want to know where our marker is... what is
            // the next blend pair to the right?
            // or in otherwords, what is the next disable marker?

            SegmentedTrackBarDLL.MarkerItem nextMarker = null;
            foreach (MarkerItem m in segtrkTime.Markers)
            {
                // Only want source markers... which are above trackbar
                if (m.BelowBar == false)
                    continue;
             
                if (m.Position > iFromPos)
                {
                    if (nextMarker != null)
                    {
                        if (m.Position < nextMarker.Position)
                            nextMarker = m;
                    }
                    else
                        nextMarker = m;
                }
            }

            return nextMarker;
        }


        public void AllignRegions()
        {
            RegionItem prev = null;

            foreach(RegionItem r in segtrkTime.Regions)
            {
                if (prev == null)
                {
                    prev = r;
                    continue;
                }

                r.Position = prev.Position + prev.Width;
                prev = r;
            }
        }

        
        public void OnMarkerPosChanged(SegmentedTrackBarDLL.MarkerItem marker)
        {
            if (m_Info == null)
                return;

            m_Info.OnMarkerPosChanged(marker);
        }

        public void OnMarkerDragStarted(SegmentedTrackBarDLL.MarkerItem marker)
        {
            if (m_Info == null)
                return;

            // When we first start, we want to ensure that
            // time has stopped and re-activate later if needed.
            if (Playing == true)
            {
                m_bRestartPlaying = true;
                Playing = false;
            }
        }

        public void OnMarkerDragEnded(SegmentedTrackBarDLL.MarkerItem marker)
        {
            if (m_Info == null)
                return;

            if (m_bRestartPlaying)
            {
                Playing = true;
                m_bRestartPlaying = false;
            }

            // We want the scrubber to be recaluclated after
            // our transition marker has changed. We need to
            // run-up time just to be sure.
            MFramework.Instance.Animation.RunUpTime(
                MFramework.Instance.Animation.CurrentTime);
            OnScrubberPosChanged();
        }

        public void OnScrubberRepositioned()
        {
            if (m_Info == null)
                return;

            // We want the scrubber to be recaluclated after
            // our transition marker has changed. We need to
            // run-up time just to be sure.
            if (m_Transition != null || m_SrcSeq != null)
            {                                                    
                MFramework.Instance.Animation.RunUpTime(
                    MFramework.Instance.Animation.CurrentTime);
                OnScrubberPosChanged();
            }
        }

        private void btnPrevFrame_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;
            segtrkTime.Scrubber.MoveLeft(segtrkTime.Increment);
        }

        private void btnNextFrame_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;
            segtrkTime.Scrubber.MoveRight(segtrkTime.Increment);
        }

        private void sbPlaybackRate_ValueChanged(object sender, decimal Value)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Clock.ScaleFactor = Convert.ToSingle(Value);
            lblPlaybackRate.Text = 
                MFramework.Instance.Clock.ScaleFactor.ToString("f2") + "x";
        }

        private void Clock_OnScaleFactorChanged(float fScaleFactor)
        {
            sbPlaybackRate.Value = Convert.ToDecimal(fScaleFactor);
        }

        private void Clock_OnEnabledChanged(bool bEnabled)
        {   
            this.Playing = bEnabled;
        }

		public void OnContextMenuClicked(Point kPoint, object kTextKey, bool bNewMode)
		{
			if (MFramework.Instance.Animation.ActiveSequence == null)
			{
				return;
			}

			cmEventMenu.MenuItems.Clear();

			if (bNewMode)
			{
				cmEventMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
                {
                    miAddEvent
                });
			}
			else
			{
				cmEventMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
                {
                    miModifyEvent,
                    miRemoveEvent,
					miRegisterMacro
                });
				cmEventMenu.Tag = kTextKey;
			}
			cmEventMenu.Show(segtrkTime, kPoint);
		}

		public void RemoveTextKeyEvent(object kObject)
		{
			if (kObject == null || !(kObject is MSequence.MTextKey))
			{
				return;
			}

			MSequence.MTextKey kTextKey = (MSequence.MTextKey)kObject;
			if (kTextKey.ExtraData == null)
			{
				return;
			}

			MAnimation.EventType eEventType = MAnimation.EventType.TextKeyEvent;
			if (kTextKey.ExtraData is MSequence.MTextKey.MEffectData)
			{
				eEventType = MAnimation.EventType.EffectEvent;
			}
			else if (kTextKey.ExtraData is MSequence.MTextKey.MAudioData)
			{
				eEventType = MAnimation.EventType.SoundEvent;
			}

			MFramework.Instance.Animation.RemoveEvent(kTextKey.Text, kTextKey.Time, eEventType);
			m_Info.Setup();
		}

		public void OnTextKeyEventRepositioned(object kObject, float fTimeRate)
		{
			if (kObject == null || !(kObject is MSequence.MTextKey))
			{
				return;
			}

			MSequence.MTextKey kTextKey = (MSequence.MTextKey)kObject;
			if (kTextKey.ExtraData == null)
			{
				return;
			}

			MSequence kSeq = MFramework.Instance.Animation.ActiveSequence;
			float fNewTime = kSeq.Length * fTimeRate;

			MAnimation.EventType eEventType = MAnimation.EventType.TextKeyEvent;
			if(kTextKey.ExtraData is MSequence.MTextKey.MEffectData)
			{
			    eEventType = MAnimation.EventType.EffectEvent;
			}
			else if(kTextKey.ExtraData is MSequence.MTextKey.MAudioData)
			{
				eEventType = MAnimation.EventType.SoundEvent;
			}

			if(MFramework.Instance.Animation.ModifyEventTime(kTextKey.Text, kTextKey.Time, fNewTime, eEventType))
			{
				foreach (MarkerItem kMarker in segtrkTime.Markers)
				{
					if (kMarker.Tag != null && kMarker.Tag == kObject)
					{
						if (eEventType == MAnimation.EventType.TextKeyEvent)
						{
							kMarker.Text = kTextKey.Text + "\nOccurs at time " + fNewTime.ToString("f3");
							kTextKey.Time = fNewTime;
							kMarker.Tag = kTextKey;
							break;
						}
						else if (eEventType == MAnimation.EventType.EffectEvent)
						{
							MSequence.MTextKey.MEffectData kEffectData = (MSequence.MTextKey.MEffectData)kTextKey.ExtraData;
							kMarker.Text = kTextKey.Text + "\nOccurs at time " + fNewTime.ToString("f3") +
								"\nEffect Name : " + kEffectData.EffectName +
								"\nAttach Point : " + kEffectData.AttachPoint +
								"\nScale : " + kEffectData.Scale.ToString() +
								"\nLoop : " + (kEffectData.Loop ? "TRUE" : "FALSE");
							
							kTextKey.Time = fNewTime;
							kMarker.Tag = kTextKey;
							break;
						}
						else if (eEventType == MAnimation.EventType.SoundEvent)
						{
							MSequence.MTextKey.MAudioData kAudioData = (MSequence.MTextKey.MAudioData)kTextKey.ExtraData;
							kMarker.Text = kTextKey.Text + "\nOccurs at time " + fNewTime.ToString("f3") +
								"\nSound Name : " + kAudioData.SoundName +
								"\nVolume : " + kAudioData.Volume.ToString() +
								"\nMinDist : " + kAudioData.MinDistance.ToString() +
								"\nMaxDist : " + kAudioData.MaxDistance.ToString();
							
							kTextKey.Time = fNewTime;
							kMarker.Tag = kTextKey;
							break;
						}
					}
				}
			}
		}

		#region Menu Item Handlers

		private void miAddEvent_Click(object sender, System.EventArgs e)
		{
			TextKeyForm kAddTextKey = new TextKeyForm();
			kAddTextKey.SetCurrentTime(MFramework.Instance.Animation.CurrentTime.ToString("f3"));
			if (kAddTextKey.ShowDialog() == DialogResult.OK && m_Info != null)
			{
				m_Info.Setup();
			}
		}

		private void miModifyEvent_Click(object sender, System.EventArgs e)
		{
			if (cmEventMenu.Tag == null || !(cmEventMenu.Tag is MSequence.MTextKey))
			{
				return;
			}

			TextKeyForm kModifyTextKey = new TextKeyForm();
			kModifyTextKey.SetEditTextKeyInfo((MSequence.MTextKey)cmEventMenu.Tag);
			if (kModifyTextKey.ShowDialog() == DialogResult.OK && m_Info != null)
			{
				m_Info.Setup();
			}
		}

		private void miRegisterMacro_Click(object sender, System.EventArgs e)
		{
			if (cmEventMenu.Tag == null || !(cmEventMenu.Tag is MSequence.MTextKey))
			{
				return;
			}

			MacroManager.Instance.OpenMacroRegisterDialog((MSequence.MTextKey)cmEventMenu.Tag);
		}

		private void miRemoveEvent_Click(object sender, System.EventArgs e)
		{
			if (MessageBox.Show("이 이벤트를 정말로 삭제하시겠습니까?", "확인", MessageBoxButtons.YesNo)
				== DialogResult.Yes)
			{
				RemoveTextKeyEvent(cmEventMenu.Tag);
			}
		}

		#endregion

        private void TimeSlider_SizeChanged(object sender, EventArgs e)
        {
            segtrkTime.Size = new Size(btnPrevFrame.Location.X - 5, segtrkTime.Size.Height);
            segtrkTime.Refresh();
        }
	}
}
