using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//class for the vegetable table
public class VegetableTable : Interactable
{
    public Vegetables vegetableType;

    private void Update()
    {
        if (player != null && Input.GetKeyUp(player.pm.interact) && !player.hasSalad)
        {
            for (int i = 0; i < player.vegetables.Length; i++)
            {
                if (player.vegetables[i].vegetable == Vegetables.None)
                {
                    player.vegetables[i].AddVegetable(vegetableType);
                    break;
                }
            }
        }
    }
}
