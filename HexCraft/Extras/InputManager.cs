using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public static class InputManager
{
    //Movement  Controls
    public static KeyCode Left = KeyCode.A;
    public static KeyCode Right = KeyCode.D;
    public static KeyCode Up = KeyCode.W;
    public static KeyCode Down = KeyCode.S;
    public static KeyCode Jump = KeyCode.Space;

    //Menu Controls
    public static KeyCode BuildingCamera = KeyCode.Tab;
    public static KeyCode Inventory = KeyCode.I;
    public static KeyCode Menu = KeyCode.Escape;
    public static KeyCode CraftingMenu = KeyCode.C;

    public static int Horizontal
    {
        get
        {
            if (Input.GetKey(Left)) return -1;
            else if (Input.GetKey(Right)) return 1;
            else return 0;
        }
    }
    public static int Vertical
    {
        get
        {
            if (Input.GetKey(Up)) return 1;
            else if (Input.GetKey(Down)) return -1;
            else return 0;
        }
    }


    public static float MouseXSpeed = 1.5f;
    public static float MouseYSpeed = 1.5f;
}
