using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName ="Custom/Items/Equipable Item")]
public class Equipable : Item
{
    [Header("Equipable Vars")]
    public SlotTypes slotType;
    public int armor = 1; //armor or attack

    public void Equip()
    {
        if (slotType != SlotTypes.None || slotType != SlotTypes.Hand)
        {
            GameManager.playerInstance.equipmentSlots[slotType] = this;
        }
    }
}
