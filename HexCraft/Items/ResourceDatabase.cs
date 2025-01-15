using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Custom/Resource Database")]
public class ResourceDatabase : ScriptableObject
{
    public List<SO_Resource> resources = new List<SO_Resource>();
}
