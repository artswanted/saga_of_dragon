using System;
using System.Collections;
using System.Reflection;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Services
{
	/// <summary>
	/// Summary description for UICommandServiceImpl.
	/// </summary>
	public class UICommandServiceImpl : IUICommandService
	{
        #region Private Data
	    Hashtable m_commands;
        #endregion
		public UICommandServiceImpl()
		{
            m_commands = new Hashtable();
		}

	    public UICommand GetCommand(string name)
	    {
            UICommand command = null;
            command = m_commands[name] as UICommand;
            if (command == null)
            {
                command = CreateCommand(name);
                AddCommand(command);
            }
            return command;
	    }

	    public UICommand[] GetAllCommands()
	    {
            UICommand[] commands = new UICommand[m_commands.Count];
            int index = 0;
            foreach (UICommand command in m_commands.Values)
            {
                commands[index++] = command;
            }
            return commands;
	    }

	    public void BindCommands(object target)
        {
            Type targetType = target.GetType();
            BindingFlags bindingFlags = BindingFlags.NonPublic | BindingFlags.Public
                | BindingFlags.Instance;
            UICommandUtility.BindMethodsOnType(targetType, bindingFlags, target);
        }

	    public void UnBindCommands(object pmTarget)
	    {
            foreach(UICommand command in m_commands.Values)
            {
                command.UnregisterAllByTarget(pmTarget);
            }	       
	    }

	    #region IService Implementation
	    public string Name
	    {
	        get { return this.GetType().ToString(); }
	    }

	    /// <summary>
	    /// Allows a service to perform initialization code
	    /// </summary>
	    /// <returns>true upon success</returns>
	    /// <remarks>A service may not assume that any other service exists
	    /// at the point this is called. The order of service intiialization 
	    /// is not fixed and may change when new plugins are added</remarks>
	    public bool Initialize()
	    {
            return true;
	    }

	    /// <summary>
	    /// Allows a service to interact with other services to perform 
	    /// start-up routines
	    /// </summary>
	    /// <returns></returns>
	    public bool Start()
	    {
            return true;
	    }
        #endregion

        private void AddCommand(UICommand command)
        {
            m_commands.Add(command.Name, command);
        }
	    private static UICommand CreateCommand(string name)
	    {
            UICommand command = new UICommand(name);
            return command;
	    }


	}
}
