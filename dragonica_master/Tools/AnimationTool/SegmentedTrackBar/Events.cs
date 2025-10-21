using System;

namespace SegmentedTrackBarDLL
{
	/// <summary>
	/// Summary description for Events.
	/// </summary>
	public class Events
	{
        public Events(){}

        #region Delegate Definitions
        public delegate void EventHandler_ScrubberPosChanged();        
        public delegate void EventHandler_MarkerPosChanged(MarkerItem marker);        
        public delegate void EventHandler_MarkerDragEnded(MarkerItem marker);
        public delegate void EventHandler_MarkerDragStarted(MarkerItem marker);
        public delegate void EventHandler_ScrubberChangedRegion(RegionItem oldRegion, RegionItem newRegion);
        public delegate void EventHandler_ScrubberRepositioned();
		public delegate void EventHandler_ContextMenu(System.Drawing.Point kPoint, object kObject, bool bNewMode);
		public delegate void EventHandler_TextKeyEventRepositioned(object kObject, float fTimeRate);

        #endregion

        #region Event Raising and Handlers
        static public event EventHandler_ScrubberPosChanged ScrubberPosChanged;       
        static public void RaiseScrubberPosChanged()
        {
            if (ScrubberPosChanged != null)
                ScrubberPosChanged();
        }
        
        static public event EventHandler_MarkerPosChanged MarkerPosChanged;       
        static public void RaiseMarkerPosChanged(MarkerItem marker)
        {
            if (MarkerPosChanged != null)
                MarkerPosChanged(marker);
        }

        static public event EventHandler_MarkerDragEnded MarkerDragEnded;       
        static public void RaiseMarkerDragEnded(MarkerItem marker)
        {
            if (MarkerDragEnded != null)
                MarkerDragEnded(marker);
        }

        static public event EventHandler_MarkerDragStarted MarkerDragStarted;       
        static public void RaiseMarkerDragStarted(MarkerItem marker)
        {
            if (MarkerDragStarted != null)
                MarkerDragStarted(marker);
        }

        static public event EventHandler_ScrubberRepositioned ScrubberRepositioned;       
        static public void RaiseScrubberRepositioned()
        {
            if (ScrubberRepositioned != null)
                ScrubberRepositioned();
        }

		static public event EventHandler_ContextMenu ContextMenuClicked;
		static public void RaiseContextMenu(System.Drawing.Point kPoint, object kObject, bool bNewMode)
		{
			if (ContextMenuClicked != null)
			{
				ContextMenuClicked(kPoint, kObject, bNewMode);
			}
		}
		static public event EventHandler_TextKeyEventRepositioned TextKeyEventRepositioned;
		static public void RaiseTextKeyEventRepositioned(object kObject, float fTimeRate)
		{
			if (TextKeyEventRepositioned != null)
			{
				TextKeyEventRepositioned(kObject, fTimeRate);
			}
		}
        #endregion
	}
}
