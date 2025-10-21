using System;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility
{
	/// <summary>
	/// Summary description for AnimationControlUtility.
	/// </summary>
	public class AnimationControlUtility
	{
		public AnimationControlUtility()
		{
		}
	    
	    [UICommandHandler("PlayAnimations")]
	    private static void OnPlayAnimations(object sender, EventArgs args)
	    {	        
            if (MFramework.Instance.IsTimePaused())
            {
                MFramework.Instance.UnPauseAnimation();
            }
            else
            {
                MFramework.Instance.PauseAnimation();
            }
	    }
	    
	    [UICommandValidator("PlayAnimations")]
	    private static void OnValidatePlayAnimations(object sender, 
	        UIState state)
	    {
	        state.Checked = !MFramework.Instance.IsTimePaused();
	    }
	    
	    [UICommandHandler("ResetAnimations")]
	    private static void OnResetAnimations(object sender, EventArgs args)
	    {
	        MFramework.Instance.RestartAnimation();
	    }
	    
	    [UICommandValidator("ResetAnimations")]
	    private static void OnValidateResetAnimations(object sender, 
	        UIState state)
	    {
	        
	    }
	}
}
