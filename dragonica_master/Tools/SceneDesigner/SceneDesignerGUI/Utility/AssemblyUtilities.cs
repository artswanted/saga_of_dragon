using System;
using System.Collections;
using System.Reflection;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
	/// <summary>
	/// Summary description for AssemblyUtilities.
	/// </summary>
	public class AssemblyUtilities
	{
		public AssemblyUtilities()
		{
		}

		public static bool CheckForInitAndShutDown(Assembly assembly)
		{
			bool found = true;
			found = found && 
				(FindAttributedMethod(assembly, typeof(DllInitAttribute)) 
				!= null);
			found = found && 
				(FindAttributedMethod(assembly, typeof(DllShutdownAttribute)) 
				!= null);
			return found;
		}

		public static bool FindAllCallInitMethod(Assembly assembly)
		{			
			return FindAndCallAttributedMethod(assembly, 
				typeof(DllInitAttribute));
		}

		public static bool FindAndCallShutdownMethod(Assembly assembly)
		{
			return FindAndCallAttributedMethod(assembly, 
				typeof(DllShutdownAttribute));			
		}

		private static bool FindAndCallAttributedMethod(Assembly assembly, 
			Type type)
		{
			MethodInfo method = FindAttributedMethod(assembly, type);
			if (method != null)
			{
				method.Invoke(null, new object[] {} );
				return true;
			}
			else
			{
				return false;
			}
		}				

		private static MethodInfo FindAttributedMethod(Assembly assembly,
			Type attributeType)
		{
			Type[] types = assembly.GetTypes();
			//there should only be one per assembly
			ArrayList initMethodList = new ArrayList();
			foreach (Type type in types)
			{
				BindingFlags bindingInfo = BindingFlags.Public 
					| BindingFlags.NonPublic | BindingFlags.Static | 
					BindingFlags.Instance | BindingFlags.DeclaredOnly;
				MethodInfo[] methods = type.GetMethods(bindingInfo);
				foreach(MethodInfo method in methods)
				{
					object[] methodAttributes = 
						method.GetCustomAttributes(
						attributeType, false);
					if (methodAttributes.Length > 0)
					{
						if (!method.IsStatic)
						{
							//Then fail, we have no instance to use
							return null;
						}
						ParameterInfo[] parameters = method.GetParameters();
						if (parameters.Length != 0)
						{
							//Then fail, we have no parameters to pass
							return null;
						}
						initMethodList.Add(method);
					}					
				}
			}
			if (initMethodList.Count == 1)
			{
				MethodInfo method = initMethodList[0] as MethodInfo;
				//method.Invoke(null, new object[]{});				
				return method;
			}
			else
			{
				//Assembly should have one and only one attributed
				return null;
			}
		   
		}
	}
}
