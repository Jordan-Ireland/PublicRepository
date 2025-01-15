using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Everything interactable uses this script for its base
/// </summary>
public class Interactable : MonoBehaviour
{
    protected Player player;

    //Activates when player steps on trigger
    protected virtual void OnTriggerStay2D(Collider2D collision)
    {
        if(player == null)
            player = collision.GetComponent<Player>();
    }

    //activates when player steps off trigger
    protected virtual void OnTriggerExit2D(Collider2D collision)
    {
        if (player != null && collision.GetComponent<Player>() == player)
        {
            player = null;
        }
    }
}
