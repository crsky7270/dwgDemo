package com.booway.dwgdemo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import junit.framework.Test;

import java.io.File;
import java.util.List;
import java.util.Vector;

public class TestActivity extends Activity {

    final static String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/download";

    private BaseAdapter mBaseAdapter;

    private List<String> fileNameList;

    private ListView lv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);
        getAllDwgFileName(path);
        lv = (ListView) findViewById(R.id.listView);
//        Button btn = (Button) findViewById(R.id.button);
//        btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent(TestActivity.this.getApplication(), MainActivity.class);
//                startActivity(intent);
//            }
//        });
        InitListView();
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                TextView tv = view.findViewById(R.id.fileName);
//                Toast.makeText(TestActivity.this,tv.getText(),Toast.LENGTH_SHORT).show();
                Bundle bundle = new Bundle();
                bundle.putString("fileName", path + "/" + tv.getText() + "");
                Intent intent = new Intent(TestActivity.this.getApplication(), MainActivity.class);
                intent.putExtras(bundle);
                startActivity(intent);
            }
        });
    }

    private void InitListView() {
        mBaseAdapter = new BaseAdapter() {
            @Override
            public int getCount() {
                return fileNameList.size();
            }

            @Override
            public String getItem(int position) {
                return fileNameList.get(position);
            }

            @Override
            public long getItemId(int position) {
                return position;
            }

            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                LayoutInflater inflater = TestActivity.this.getLayoutInflater();
                View view;
                if (convertView == null) {
                    view = inflater.inflate(R.layout.item, null);
                } else {
                    view = convertView;
                }
                TextView tv = view.findViewById(R.id.fileName);
                tv.setText(fileNameList.get(position));
                return view;
            }
        };
        lv.setAdapter(mBaseAdapter);
    }

    public void getAllDwgFileName(String fileAbsolutePath) {
        fileNameList = new Vector<>();
        File dir = new File(fileAbsolutePath);
        File[] files = dir.listFiles();

        for (int iFlieLength = 0; iFlieLength < files.length; iFlieLength++) {
            String prefix = files[iFlieLength].getName().toLowerCase()
                    .substring(files[iFlieLength].getName().lastIndexOf(".") + 1);
            if (!files[iFlieLength].isDirectory() && prefix.equals("dwg")) {
                fileNameList.add(files[iFlieLength].getName());
            }
        }
    }

}
