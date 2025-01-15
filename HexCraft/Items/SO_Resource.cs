using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Custom/Resource SO")]
public class SO_Resource : ScriptableObject
{
    public int maxHealth;
    public int expToGive;
    public ResourceType resourceType;
    public int itemToGive;
    public Vector2 amountRange = new Vector2(5, 10);
    public float timeToReset = 120;
    public bool instant = false;
    public ItemProperty itemProperty;
}
