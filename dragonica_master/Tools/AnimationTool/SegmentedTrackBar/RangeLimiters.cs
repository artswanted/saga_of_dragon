using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using System.ComponentModel.Design;

namespace SegmentedTrackBarDLL
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class RangeLimiter
    {
        private int m_iPosition;
        private bool m_bEnabled;
        private bool m_bVisible;

        public RangeLimiter()
        {
            m_iPosition = 0;
            m_bEnabled = true;
            m_bVisible = true;
        }
        
        public int Position
        {
            get { return m_iPosition; }
            set { m_iPosition = value; }
        }
        
        public bool Enabled
        {
            get { return m_bEnabled; }
            set { m_bEnabled = value; }
        }

        public bool Visible
        {
            get { return m_bVisible; }
            set { m_bVisible = value; }
        }
    }
}
