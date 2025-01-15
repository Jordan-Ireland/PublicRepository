using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// class for the trashcan
/// </summary>
public class Trashcan : Interactable
{
    private void Update()
    {
        if(player!= null && player.hasSalad && Input.GetKey(player.pm.interact))
        {
            player.RemoveSalad();
        }
    }
}
