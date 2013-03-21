package generators;

import java.io.*;
import java.util.Scanner;
import java.util.regex.*;
import java.util.*;


public class ObjFile 
{
//	public static final GPUFloat24 default_color = GPUFloat24.parseDouble(1.0);
//	public static final GPUFloat24 default_perp = GPUFloat24.parseDouble(0.0);
//
//	Scanner objScanner;
//
//	/** Contains list of all positions */
//	ArrayList<GPUFloat24[]> positions;
//	/** Contains list of all normals */
//	ArrayList<GPUFloat24[]> normals;
//	/** Contains list of all triangles */
//	ArrayList<Triangle> triangles;
//	HashMap<Vertex, Short> MappedVertices;
//	ArrayList<Vertex> vertices;
//	
//	/** .OBJ File Parser that Dumps out Vertices According to the GPU format */
//	public static void main (String[] args)
//	{
//		if (args.length != 3)
//		{
//			System.out.println("Usage: ObjFile [Obj File] [Vertices Output File] [Triangles Output File]");
//			System.exit(0);
//		}
//		ObjFile objFile = new ObjFile(new File(args[0]));
//		objFile.dumpRawVertices(new File(args[1]));
//		objFile.dumpTriangles(new File(args[2]));
//	}
//	
//	public ObjFile (File objFile)
//	{
//		try
//		{
//			objScanner = new Scanner(objFile);
//		} catch (Exception e)
//		{
//			System.out.println(e);
//		}
//		positions = new ArrayList<GPUFloat24[]>();
//		normals = new ArrayList<GPUFloat24[]>();
//		triangles = new  ArrayList<Triangle>();
//		
//		MappedVertices = new HashMap<Vertex, Short>();
//		vertices = new ArrayList<Vertex>();
//		
//		parseObjFile();
//	}
//	
//	public void parseObjFile()
//	{
//		int positionNum = 0;
//		int normalNum = 0;
//		int triangleNum = 0;
//		int vertexNum = 0;
//		
//		while (objScanner.hasNext())
//		{
//			String parseLine = objScanner.nextLine();
//			
//			Pattern vertexPattern = Pattern.compile ("v\\s+(\\S*)\\s+(\\S*)\\s+(\\S*)");
//			Pattern normalPattern = Pattern.compile ("vn\\s+(\\S*)\\s+(\\S*)\\s+(\\S*)");
//			Pattern trianglePattern = Pattern.compile ("f\\s+(\\S*)/(\\S*)/(\\S*)\\s+(\\S*)/(\\S*)/(\\S*)\\s+(\\S*)/(\\S*)/(\\S*)\\s*");
//			Matcher vertexMatcher = vertexPattern.matcher(parseLine);
//			Matcher normalMatcher = normalPattern.matcher(parseLine);
//			Matcher triangleMatcher = trianglePattern.matcher(parseLine);
//			if (vertexMatcher.matches())
//			{
//				GPUFloat24 x = GPUFloat24.parseDouble(Double.parseDouble(vertexMatcher.group(1)));
//				GPUFloat24 y = GPUFloat24.parseDouble(Double.parseDouble(vertexMatcher.group(2)));
//				GPUFloat24 z = GPUFloat24.parseDouble(Double.parseDouble(vertexMatcher.group(3)));
//				GPUFloat24 w = GPUFloat24.parseDouble(1.0);
//				
//				GPUFloat24[] position = new GPUFloat24[] {x, y, z, w};
//				
//				positions.add(positionNum, position);
//				positionNum++;
//			}
//			if (normalMatcher.matches())
//			{
//				GPUFloat24 nx = GPUFloat24.parseDouble(Double.parseDouble(normalMatcher.group(1)));
//				GPUFloat24 ny = GPUFloat24.parseDouble(Double.parseDouble(normalMatcher.group(2)));
//				GPUFloat24 nz = GPUFloat24.parseDouble(Double.parseDouble(normalMatcher.group(3)));
//				GPUFloat24 nw = GPUFloat24.parseDouble(1.0);
//				
//				GPUFloat24[] normal = new GPUFloat24[] {nx, ny, nz, nw};
//				normals.add(normalNum, normal);
//				normalNum++;
//			}
//			if (triangleMatcher.matches())
//			{
//				short p1 = Short.parseShort(triangleMatcher.group(1));
//				short p2 = Short.parseShort(triangleMatcher.group(4));
//				short p3 = Short.parseShort(triangleMatcher.group(7));
//				short n1 = Short.parseShort(triangleMatcher.group(3));
//				short n2 = Short.parseShort(triangleMatcher.group(6));
//				short n3 = Short.parseShort(triangleMatcher.group(9));
//				
//				//Subtracted 1 because vertices start at 1 for some reason
//				GPUFloat24[] pos1 = positions.get(p1 - 1);
//				GPUFloat24[] pos2 = positions.get(p2 - 1);
//				GPUFloat24[] pos3 = positions.get(p3 - 1);
//
//				GPUFloat24[] col1 = new GPUFloat24[] {default_color, default_color, default_color, default_color};
//				GPUFloat24[] col2 = new GPUFloat24[] {default_color, default_color, default_color, default_color};
//				GPUFloat24[] col3 = new GPUFloat24[] {default_color, default_color, default_color, default_color};
//
//				GPUFloat24[] nor1 = normals.get(n1 - 1);
//				GPUFloat24[] nor2 = normals.get(n2 - 1);
//				GPUFloat24[] nor3 = normals.get(n3 - 1);
//
//				GPUFloat24[] per1 = new GPUFloat24[] {default_perp, default_perp, default_perp, default_perp};
//				GPUFloat24[] per2 = new GPUFloat24[] {default_perp, default_perp, default_perp, default_perp};
//				GPUFloat24[] per3 = new GPUFloat24[] {default_perp, default_perp, default_perp, default_perp};
//				
//				Vertex v1 = new Vertex((short) 0, pos1, col1, nor1, per1);
//				Vertex v2 = new Vertex((short) 0, pos2, col2, nor2, per2);
//				Vertex v3 = new Vertex((short) 0, pos3, col3, nor3, per3);
//				
//				short ID1;
//				short ID2;
//				short ID3;
//				
//				if (!MappedVertices.containsKey(v1))
//				{
//					ID1 = (short) vertexNum;
//					v1.ID = ID1;
//					MappedVertices.put(v1, ID1);
//					vertices.add(v1);
//					vertexNum++;
//				}
//				else ID1 = (short) MappedVertices.get(v1);
//				
//				if (!MappedVertices.containsKey(v2))
//				{
//					ID2 = (short) vertexNum;
//					v2.ID = ID2;
//					MappedVertices.put(v2, ID2);
//					vertices.add(v2);
//					vertexNum++;
//				}
//				else ID2 = (short) MappedVertices.get(v2);
//
//				if (!MappedVertices.containsKey(v3))
//				{
//					ID3 = (short) vertexNum;
//					v3.ID = ID3;
//					MappedVertices.put(v3, ID3);
//					vertices.add(v3);
//					vertexNum++;
//				}
//				else ID3 = (short) MappedVertices.get(v3);
//
//				Triangle newTriangle = new Triangle((short)triangleNum, ID1, ID2, ID3);
//				triangles.add(newTriangle);
//				triangleNum++;
//			}
//
//		}	
//	}
//	
//	public void dumpRawVertices(File outVerticesFile)
//	{		
//		try
//		{
//			FileWriter writer = new FileWriter(outVerticesFile);
//			for (int i = 0; i < vertices.size(); i++)
//				writer.write(vertices.get(i).toHexString() + "\n");
//			writer.close();
//		} 
//		catch (Exception e)
//		{
//			throw new Error(e.toString());
//		}		
//	}
//
//	public void dumpTriangles(File outTrianglesFile)
//	{		
//		try
//		{
//			FileWriter writer = new FileWriter(outTrianglesFile);
//			for (int i = 0; i < triangles.size(); i++)
//			{
//				Triangle t = triangles.get(i);				
//				writer.write(String.format("%04x_%04x_%04x_%04x\n",
//					i, t.v1, t.v2, t.v3));
//			}
//			writer.close();
//		} 
//		catch (Exception e)
//		{
//			throw new Error(e);
//		}		
//	}

	
}
