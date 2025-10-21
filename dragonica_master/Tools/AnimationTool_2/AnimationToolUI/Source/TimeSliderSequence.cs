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
using System.Diagnostics;

using SegmentedTrackBarDLL;
using NiManagedToolInterface;

namespace AnimationToolUI
{
	/// <summary>
	/// Summary description for TimeSliderSequence.
	/// </summary>
	public class TimeSliderSequence : TimeSliderTransitionInfo
	{
		#region Delegates
		private AppEvents.EventHandler_TimeSliderChanged m_TimeSliderChanged = null;

		private void RegisterDelegates()
		{
			UnregisterDelegates();

			// Create delegates.
			m_TimeSliderChanged = new AppEvents.EventHandler_TimeSliderChanged(OnTimeSliderChanged);

			// Register delegates.
			AppEvents.TimeSliderChanged += m_TimeSliderChanged;
		}

		private void UnregisterDelegates()
		{
			if (m_TimeSliderChanged != null)
				AppEvents.TimeSliderChanged -= m_TimeSliderChanged;
		}

		#endregion

		public TimeSliderSequence(TimeSlider ts) : base(ts)
		{
			RegisterDelegates();
		}

		~TimeSliderSequence()
		{
			UnregisterDelegates();
		}

		public void OnTimeSliderChanged()
		{
			Setup();
		}
        public override void Setup()
        {
            SrcSeq = MFramework.Instance.Animation.ActiveSequence;
            SrcSeqLength = SrcSeq.Length;

            SegTrk.ClearRegions();
            SegTrk.ClearMarkers();

            BeginTime = 0; 
            EndTime = SrcSeq.Length; 

            SegTrk.MinRange = 0;
            if (BeginTime == EndTime)
                SegTrk.MaxRange = 0;
            else
                SegTrk.MaxRange = TrackRange;

            // Add one big region...
            string strText = SrcSeq.Name + "\n" + strFullSeq +
                SrcSeq.EndKeyTimeDivFreq.ToString("f3");
            SegTrk.AddRegion(SegTrk.MinRange,
                SegTrk.MaxRange, ColorSrcA, ColorSrcB, strText, -1);

            TotalTime = SrcSeqLength;
            SetTrackerIncrement();
            SegTrk.UseLevelDividers = false;           

            // If the scrubber is not enabled, the sequence is playing. In
            // this case, reset the sequence's default position time to be
            // the beginning of the sequence. This prevents clamped sequences
            // from not animating when they are clicked if time has gone past
            // the end of the sequence.
            if (!m_ts.segtrkTime.Scrubber.Enabled)
            {
                SrcSeq.DefaultPositionTime = SrcSeq.BeginKeyTime;
            }

            // Set position from saved values
            int iValue = (int) (SrcSeq.DefaultPositionTime
                * TrackRangeOverTotalTime);
            if (iValue >= TrackRange)
                iValue = TrackRange - 1;
            SegTrk.Scrubber.Position = iValue;
            MFramework.Instance.Animation.CurrentTime = SrcSeq.DefaultPositionTime;       

            AddTextKeyMarkers();
        }

        public void AddTextKeyMarkers()
        {
            MSequence.MTextKey[] aKeys = SrcSeq.TextKeys;

            // Add markers for textkeys...
            foreach (MSequence.MTextKey tk in aKeys)
            {
                float fSrcTime = tk.Time;
                fSrcTime = GetTimeDivFreq(SrcSeq, fSrcTime);

                string strText = tk.Text + 
                    "\nOccurs at time " + fSrcTime.ToString("f3");

                MarkerItem mi;
                mi = SegTrk.AddMarker(0, strText, false, false);
				
				mi.Position = (int)(fSrcTime * TrackRangeOverTotalTime);
				if (tk.ExtraData != null)
				{
					mi.RangeRight = 9999;
					mi.Tag = tk;
					if (tk.ExtraData is MSequence.MTextKey.MEffectData)
					{
						MSequence.MTextKey.MEffectData kEffectData = (MSequence.MTextKey.MEffectData)tk.ExtraData;
						mi.Text += "\nEffect Name : " + kEffectData.EffectName +
							"\nAttach Point : " + kEffectData.AttachPoint +
							"\nScale : " + kEffectData.Scale.ToString() +
							"\nLoop : " + (kEffectData.Loop ? "TRUE" : "FALSE");

					}
					else if (tk.ExtraData is MSequence.MTextKey.MAudioData)
					{
						MSequence.MTextKey.MAudioData kAudioData = (MSequence.MTextKey.MAudioData)tk.ExtraData;
						mi.Text += "\nSound Name : " + kAudioData.SoundName +
							"\nVolume : " + kAudioData.Volume.ToString() +
							"\nMinDist : " + kAudioData.MinDistance.ToString() +
							"\nMaxDist : " + kAudioData.MaxDistance.ToString();
					}
				}
            }
        }
	}
}
